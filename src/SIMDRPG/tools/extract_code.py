#!/usr/bin/env python3
"""从讲义提取完整文件到 verify/full。

讲义里每个"完整文件"用如下标记开头：

    <!-- FILE: include/platform/Window.h -->

紧接着一个 cpp 代码块，块内容就是该文件的**全部内容**（可直接保存编译）。

本脚本扫描 docs/*.md，把所有标记过的代码块写到 verify/full/ 下，
从而保证「讲义」与「可编译实现」是同一份代码，不存在不一致。

用法：
    python tools/extract_code.py            # 写入 verify/full
    python tools/extract_code.py --check    # 只检查，不写（CI 用，退出码非0表示不一致）
"""
import os
import re
import sys
import glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCS = sorted(glob.glob(os.path.join(ROOT, "docs", "*.md")))
OUT = os.path.join(ROOT, "verify", "full")

# 只认带扩展名的真实路径，避免把正文里的示例标记也匹配进来
MARKER = re.compile(
    r"<!--\s*FILE:\s*([A-Za-z0-9_][\w./\-]*\.(?:h|cpp|txt|cmake))\s*-->")
BLOCK = re.compile(r"```[a-zA-Z+]*\n(.*?)```", re.S)

# 这些文件属于仓库根目录，不是 verify/full
ROOT_FILES = {"CMakeLists.txt"}


def collect():
    """返回 {相对路径: (内容, 来源文档, 行号)}，并检测重复定义。"""
    found = {}
    dup = []
    for doc in DOCS:
        text = open(doc, encoding="utf-8").read()
        for m in MARKER.finditer(text):
            rel = m.group(1).strip()
            rest = text[m.end():]
            bm = BLOCK.search(rest)
            if not bm:
                print(f"[错误] {os.path.basename(doc)}: {rel} 标记后没有代码块")
                continue
            # 标记与代码块之间只允许空白
            between = rest[:bm.start()]
            if between.strip():
                print(f"[警告] {os.path.basename(doc)}: {rel} 标记与代码块之间有正文，已忽略")
            line_no = text[:m.start()].count("\n") + 1
            body = bm.group(1)
            # 去掉块首尾多余空行，保证以单个 \n 结尾
            body = body.strip("\n") + "\n"
            if rel in found:
                dup.append((rel, found[rel][1], os.path.basename(doc)))
            found[rel] = (body, os.path.basename(doc), line_no)
    return found, dup


def main():
    check_only = "--check" in sys.argv
    found, dup = collect()

    print(f"讲义中标记的完整文件：{len(found)} 个")
    for rel, (body, doc, ln) in sorted(found.items()):
        n = body.count("\n")
        print(f"  {rel:44s} {n:4d} 行  ({doc}:{ln})")

    if dup:
        print("\n[错误] 同一路径被多次定义：")
        for rel, a, b in dup:
            print(f"  {rel}  冲突于 {a} 与 {b}")
        return 2

    # 与磁盘对比
    diff = []
    for rel, (body, doc, ln) in sorted(found.items()):
        path = os.path.join(ROOT if rel in ROOT_FILES else OUT,
                            rel.replace("/", os.sep))
        old = open(path, encoding="utf-8-sig").read() if os.path.exists(path) else None
        if old is None:
            diff.append((rel, "缺失"))
        elif old.strip("\n") != body.strip("\n"):
            diff.append((rel, "内容不同"))

    # 磁盘上有、讲义里没标记的（这些是"讲义没教"的文件）
    taught = {rel.replace("/", os.sep) for rel in found if rel not in ROOT_FILES}
    # 只扫 include/ 和 src/ —— build/ 里是 SDL 源码，tests/ 是验证程序，
    # 它们本来就不该出现在讲义里。
    orphan = []
    for sub in ("include", "src"):
        for base, _, fs in os.walk(os.path.join(OUT, sub)):
            for f in fs:
                if not f.endswith((".cpp", ".h")):
                    continue
                p = os.path.join(base, f)
                rel = os.path.relpath(p, OUT)
                if rel not in taught:
                    orphan.append(rel.replace(os.sep, "/"))

    if orphan:
        print("\n[错误] verify/full 里存在但讲义未收录（学员抄不到）：")
        for o in orphan:
            print("  ", o)

    if diff:
        print("\n[需要同步]")
        for rel, why in diff:
            print(f"  {why}: {rel}")

    if check_only:
        return 1 if (diff or orphan or dup) else 0

    if not diff and not orphan:
        print("\n讲义与 verify/full 完全一致。")
        return 0

    for rel, (body, doc, ln) in sorted(found.items()):
        path = os.path.join(ROOT if rel in ROOT_FILES else OUT,
                            rel.replace("/", os.sep))
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(body)
        print("写入", rel)
    print(f"\n已从讲义导出 {len(found)} 个文件到 verify/full/")
    return 0


if __name__ == "__main__":
    sys.exit(main())
