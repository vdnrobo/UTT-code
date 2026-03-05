import sys
from pathlib import Path

sys.stdout = open("out.txt", "wt", encoding='utf-8')

DIR = Path(".")

target_ext = (
    "h", "hpp", "cpp", "md", "ino"
)

for ext in target_ext:
    for file in DIR.rglob(f"*.{ext}"):
        try:
            print(f"{file.relative_to(DIR)}")
            print(f"```{ext}")
            print(file.read_text(encoding='utf-8', errors='ignore').rstrip())
            print("```\n")
        except:
            pass
