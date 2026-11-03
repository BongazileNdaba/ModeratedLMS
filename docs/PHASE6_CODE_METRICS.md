# Phase 6 â€” Code Metrics Report

**Source snapshot:** ModeratedLMS v1.1 project archive inspected for this revision.
**Collection command:** `python3 scripts/collect_metrics.py`
**Metric script:** `scripts/collect_metrics.py`

## Source size

| Metric | Result |
|---|---:|
| C++ source/header files scanned | 15 |
| Physical lines | 1,571 |
| Nonblank lines (includes comments and preprocessor lines) | 1,295 |

These counts cover `src/` and `include/`; they exclude tests, documentation, CMake and workflow YAML. Nonblank lines are not the same as executable LOC.

## Approximate cyclomatic complexity

The supplied script estimates McCabe complexity by starting at 1 and adding occurrences of `if`, `for`, `while`, `case`, `catch`, `&&` and `||` inside detected function bodies.

| Function | Estimated complexity |
|---|---:|
| `WorkflowService::recordMark` | 14 |
| `WorkflowService::submit` | 12 |
| `UserRepository::createUser` | 11 |
| `WorkflowService::grantIndividualExtension` | 10 |
| `UserRepository::findByUsername` | 9 |
| `WorkflowService::createAssignment` | 9 |
| `WorkflowService::grantClassExtension` | 9 |
| `UserRepository::setUserActive` | 8 |
| `WorkflowService::moderateMark` | 8 |
| `PasswordHasher::verifyPassword` | 7 |

## Interpretation

The most complex functions are concentrated in submission, marking, extension and user-persistence workflows. These are appropriate candidates for smaller helper functions and more focused tests. In particular, `recordMark` (estimated 14) and `submit` (estimated 12) should be considered for decomposition after behaviour is protected by tests.

**Metric limitation:** the script is a lightweight lexical estimator, not a full C++ parser. It can miss or misidentify functions and may over/under-count decision points in templates, lambdas, macros and nested constructs. These values are indicative and should be labelled as estimates in the report. CI stores the raw output as `source-metrics.txt` so the exact snapshot can be compared over time.
