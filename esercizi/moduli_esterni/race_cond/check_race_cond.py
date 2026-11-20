import re
import sys
from pathlib import Path

VAR_RE = re.compile(r"variabile\s*=\s*(-?\d+)")
PLUS1_RE = re.compile(r"variabile\+1\s*=\s*(-?\d+)")


def analyze_file(path: Path) -> list[str]:
    errors: list[str] = []
    current_var: int | None = None
    current_plus1: int | None = None
    entry_idx = 0

    with path.open("r", encoding="utf-8", errors="replace") as f:
        for line_no, raw_line in enumerate(f, start=1):
            line = raw_line.strip()
            if not line:
                continue

            m_var = VAR_RE.search(line)
            if m_var:
                current_var = int(m_var.group(1))
                continue

            m_plus1 = PLUS1_RE.search(line)
            if m_plus1:
                current_plus1 = int(m_plus1.group(1))
                continue

            if set(line) == {"-"} and len(line) >= 5:
                entry_idx += 1

                if current_var is None or current_plus1 is None:
                    errors.append(
                        f"[{path}:{line_no}] Entry #{entry_idx} incompleta o malformata: "
                        f"variabile={current_var}, variabile+1={current_plus1}"
                    )
                elif current_plus1 != current_var + 1:
                    errors.append(
                        f"[{path}:{line_no}] Errore interno nell'entry #{entry_idx}: "
                        f"\tatteso variabile+1={current_var + 1}, trovato {current_plus1}"
                    )

                current_var = None
                current_plus1 = None

    if current_var is not None or current_plus1 is not None:
        entry_idx += 1
        errors.append(
            f"[{path}] Entry #{entry_idx} troncata (manca separatore '---'): "
            f"variabile={current_var}, variabile+1={current_plus1}"
        )

    return errors


def main() -> int:
    if len(sys.argv) < 2:
        print("Uso: python3 check_race.py <file1.log> [file2.log ...]")
        return 2

    total_errors = 0
    for arg in sys.argv[1:]:
        path = Path(arg)
        if not path.is_file():
            print(f"File non trovato: {path}")
            total_errors += 1
            continue

        file_errors = analyze_file(path)
        if file_errors:
            total_errors += len(file_errors)
            print(f"❌ {path}: trovate {len(file_errors)} incoerenze interne")
            for err in file_errors:
                print(f"  - {err}")
        else:
            print(f"✅ {path}: nessuna incoerenza interna rilevata")

    return 1 if total_errors > 0 else 0


if __name__ == "__main__":
    raise SystemExit(main())