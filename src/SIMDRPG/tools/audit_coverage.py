#!/usr/bin/env python3
"""讲义覆盖率审计：verify/full 的每个实现文件，有多少代码行能在讲义里找到。

用法：python tools/audit_coverage.py
输出：每个文件的覆盖率 + 缺失代码行（前若干条）
"""
import os
import re
import sys
import glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCS = sorted(glob.glob(os.path.join(ROOT, "docs", "*.md"))) + [os.path.join(ROOT, "README.md")]
SRC = os.path.join(ROOT, "verify", "full")


def norm(line: str) -> str:
    """归一化：去行尾空白、压缩内部空白、去掉行注释后半段。"""
    s = line.strip()
    if not s:
        return ""
    # 去掉 // 注释（但不是 http:// 这种）
    cut = s.find("//")
    if cut > 0 and s[cut - 1] != ":":
        s = s[:cut].rstrip()
    s = re.sub(r"\s+", " ", s)
    return s


TRIVIAL = {"{", "}", "};", "public:", "private:", "protected:", "#pragma once", "else"}


def significant(line: str) -> bool:
    s = norm(line)
    if len(s) < 4:
        return False
    if s in TRIVIAL:
        return False
    if s in ("#include",):
        return False
    return True


def load_doc_text():
    """把所有文档拼成一个大字符串（用于子串查找），并记录代码块集合。"""
    whole = []
    blocks = []
    for p in DOCS:
        if not os.path.exists(p):
            continue
        t = open(p, encoding="utf-8").read()
        whole.append(t)
        for m in re.finditer(r"```[a-zA-Z+]*\n(.*?)```", t, re.S):
            blocks.append(m.group(1))
    return "\n".join(whole), blocks


def main():
    whole, blocks = load_doc_text()
    norm_whole = "\n".join(norm(l) for l in whole.split("\n"))
    block_text = ["\n".join(norm(l) for l in b.split("\n")) for b in blocks]

    files = []
    # 只扫参考实现的 include/ 和 src/（build/ 里是 SDL 源码）
    for sub in ("include", "src"):
        for base, _, fs in os.walk(os.path.join(SRC, sub)):
            for f in fs:
                if f.endswith((".cpp", ".h")):
                    files.append(os.path.join(base, f))
    files.sort()

    rows = []
    total_sig = total_hit = 0
    for path in files:
        rel = os.path.relpath(path, ROOT).replace("\\", "/")
        lines = open(path, encoding="utf-8", errors="replace").read().split("\n")
        sig = [norm(l) for l in lines if significant(l)]
        if not sig:
            continue
        miss = []
        hit = 0
        for s in sig:
            if s in norm_whole:
                hit += 1
            else:
                miss.append(s)
        cov = hit * 100.0 / len(sig)
        rows.append((cov, rel, len(sig), miss))
        total_sig += len(sig)
        total_hit += hit

    rows.sort()
    print("=" * 78)
    print(f"{'覆盖率':>7}  {'文件':46} {'有效行':>5} {'缺行':>4}")
    print("=" * 78)
    for cov, rel, n, miss in rows:
        flag = "  <-- 缺" if cov < 99 else ""
        print(f"{cov:6.1f}%  {rel:46} {n:5d} {len(miss):4d}{flag}")
    print("=" * 78)
    print(f"总覆盖率 {total_hit*100.0/total_sig:.1f}%  ({total_hit}/{total_sig} 有效行)")

    bad = [r for r in rows if r[0] < 99]
    if bad:
        print()
        print("### 缺失代码行明细 ###")
        for cov, rel, n, miss in bad:
            print(f"\n--- {rel}  (覆盖 {cov:.1f}%) ---")
            for s in miss[:40]:
                print("   ", s[:150])
            if len(miss) > 40:
                print(f"    ... 还有 {len(miss)-40} 行")
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
