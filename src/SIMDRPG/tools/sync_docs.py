import os
import re
import glob

DOCS = sorted(glob.glob('docs/*.md'))
MARK = re.compile(r'<!--[ ]*FILE:[ ]*([A-Za-z0-9_][A-Za-z0-9_./-]*[.](?:h|cpp|txt|cmake))[ ]*-->')
BLOCK = re.compile(r'```[a-zA-Z+]*\n(.*?)```', re.S)

changed = []
for doc in DOCS:
    text = open(doc, encoding='utf-8').read()
    out = []
    pos = 0
    modified = False
    for m in MARK.finditer(text):
        rel = m.group(1)
        # CMakeLists.txt 属于仓库根目录，不在 verify/full
        src = rel if rel == 'CMakeLists.txt' else os.path.join('verify/full', rel)
        if not os.path.exists(src):
            continue
        body = open(src, encoding='utf-8').read().rstrip('\n') + '\n'
        bm = BLOCK.search(text, m.end())
        if not bm:
            continue
        if text[bm.start(1):bm.end(1)] == body:
            continue
        out.append(text[pos:bm.start(1)])
        out.append(body)
        pos = bm.end(1)
        modified = True
        changed.append((doc, rel))
    if modified:
        out.append(text[pos:])
        open(doc, 'w', encoding='utf-8').write(''.join(out))

for d, r in changed:
    print('synced', d, '<-', r)
print('total', len(changed))
