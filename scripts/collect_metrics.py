#!/usr/bin/env python3
"""Collect reproducible source-size and approximate complexity metrics.

Cyclomatic complexity is estimated from C++ function bodies by counting common
control-flow keywords and logical operators. This lightweight estimator is
not a substitute for clang-tidy, lizard, or a compiler AST-based metric tool.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
SOURCE_DIRS = [ROOT / "src", ROOT / "include"]
SOURCE_EXTENSIONS = {".cpp", ".h", ".hpp"}
DECISIONS = re.compile(r"\b(if|for|while|case|catch)\b|&&|\|\|")
FUNCTION_START = re.compile(
    r"(?:^|\n)\s*(?:[\w:<>,*&~]+\s+)+(?P<name>[\w:~]+)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?\{",
    re.MULTILINE,
)


def strip_comments_and_strings(text: str) -> str:
    # Preserve line structure while removing comments and string/char literals.
    pattern = re.compile(
        r'//[^\n]*|/\*[\s\S]*?\*/|R"[^(]*\([\s\S]*?\)[^"]*"|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
    )
    return pattern.sub(lambda m: "\n" * m.group(0).count("\n"), text)


def function_metrics(text: str):
    clean = strip_comments_and_strings(text)
    results = []
    for match in FUNCTION_START.finditer(clean):
        start = match.end() - 1
        depth = 0
        end = start
        for i in range(start, len(clean)):
            if clean[i] == "{":
                depth += 1
            elif clean[i] == "}":
                depth -= 1
                if depth == 0:
                    end = i + 1
                    break
        body = clean[start:end]
        complexity = 1 + len(DECISIONS.findall(body))
        results.append((match.group("name"), complexity))
    return results


def main():
    files = sorted(
        p for folder in SOURCE_DIRS for p in folder.rglob("*")
        if p.is_file() and p.suffix in SOURCE_EXTENSIONS
    )
    total_lines = 0
    code_lines = 0
    functions = []
    print("Moderated LMS source metrics")
    print("Metric method: physical lines; nonblank, non-comment lines; approximate McCabe complexity.\n")
    for path in files:
        text = path.read_text(encoding="utf-8", errors="replace")
        lines = text.splitlines()
        nonblank = [line for line in lines if line.strip()]
        total_lines += len(lines)
        code_lines += len(nonblank)
        metrics = function_metrics(text)
        functions.extend((str(path.relative_to(ROOT)), name, cc) for name, cc in metrics)
        print(f"{path.relative_to(ROOT)}: physical={len(lines)}, nonblank={len(nonblank)}, functions_detected={len(metrics)}")
    print(f"\nTOTAL: files={len(files)}, physical_lines={total_lines}, nonblank_lines={code_lines}")
    print("\nApproximate cyclomatic complexity (highest first):")
    for path, name, cc in sorted(functions, key=lambda x: x[2], reverse=True)[:15]:
        print(f"{cc:>3}  {path} :: {name}")
    print("\nCaveat: comments/strings are stripped heuristically; C++ templates, lambdas, macros, overloaded operators and nested declarations can affect detection. Treat these as indicative metrics, not compiler-certified values.")


if __name__ == "__main__":
    main()
