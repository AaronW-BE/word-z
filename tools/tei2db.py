import sqlite3
import xml.etree.ElementTree as ET

# 1. 解析 TEI 文件
tree = ET.parse("eng-zho.tei")
root = tree.getroot()
ns = {"tei": "http://www.tei-c.org/ns/1.0"}  # 命名空间

# 2. 连接 SQLite
conn = sqlite3.connect("eng-zho.db")
cursor = conn.cursor()

# 3. 建表
cursor.execute("""
CREATE TABLE IF NOT EXISTS dictionary (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    word TEXT NOT NULL,
    pron TEXT,
    pos TEXT,
    translations TEXT,
    definition TEXT
);
""")

# 4. 遍历词条
for entry in root.findall(".//tei:entry", ns):
    word_elem = entry.find(".//tei:orth", ns)
    if word_elem is None:
        continue
    word = word_elem.text.strip()

    # 音标
    prons = [p.text.strip() for p in entry.findall(".//tei:pron", ns) if p.text]
    pron = "; ".join(prons) if prons else None

    # 词性
    pos_elem = entry.find(".//tei:pos", ns)
    pos = pos_elem.text.strip() if pos_elem is not None else None

    # 中文翻译 (quote)
    quotes = [q.text.strip() for q in entry.findall(".//tei:cit[@type='trans']/tei:quote", ns) if q.text]
    translations = "; ".join(quotes) if quotes else None

    # 英文详细释义 (def)
    defs = [d.text.strip() for d in entry.findall(".//tei:def", ns) if d.text]
    definition = " ".join(defs) if defs else None

    # 插入数据库
    cursor.execute("""
        INSERT INTO dictionary (word, pron, pos, translations, definition)
        VALUES (?, ?, ?, ?, ?)
    """, (word, pron, pos, translations, definition))

# 5. 保存并关闭
conn.commit()
conn.close()

print("✅ 转换完成，已生成 eng-zho.db")
