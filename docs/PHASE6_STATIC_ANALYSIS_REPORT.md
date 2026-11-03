# Phase 6 â€” Static Analysis Report

## Tool and configuration

**Selected analyser:** cppcheck (open-source C/C++ static analyser).
**Configured checks:** warning, performance and portability.
**CI command:**

```bash
cppcheck --enable=warning,performance,portability \
  --inline-suppr --suppress=missingIncludeSystem --error-exitcode=1 \
  -I include src include main.cpp
```

The GitHub Actions workflow installs cppcheck and runs this command on pushes and pull requests.

## Execution status

| Evidence item | Status |
|---|---|
| Analyser selected and command documented | Complete |
| CI step configured | Complete |
| cppcheck executed against this snapshot in the preparation environment | Not run: cppcheck executable was not installed; package installation timed out |
| Findings count and severity | Pending actual cppcheck output |
| Fixes/suppressions verified by rerun | Pending actual cppcheck output |

**Do not report zero findings.** No analyser output was available during preparation, so the finding count is unknown. After the first CI run, paste the complete cppcheck output here and update this report with the exact tool version, command, date, finding IDs/severities, disposition, and rerun result.

## Source-review observations (not cppcheck findings)

The following observations were found during a manual source inspection and must not be mislabelled as analyser output:

1. `WorkflowService.cpp`: `validDateTime()` checks only the string length and the positions of two hyphens. It does not fully validate calendar dates or timestamp format. Add strict parsing and tests for impossible dates (e.g. invalid month/day) before relying on lexical comparisons.
2. `WorkflowService` constructor: schema migration calls to `execute()` are not checked or surfaced to the caller. A migration failure can leave the service running against an incomplete schema. Make initialization explicit and fail construction/service setup when migration fails.
3. `WorkflowService::hasRole()` and `isEnrolled()` return `false` both when a record is absent and when a query cannot be prepared/executed. Consider a result type that distinguishes access denial from database failure.
4. `WorkflowService::submit()` stores a file path, not the uploaded file bytes. The current implementation should be described as path registration, not complete file upload/storage.
5. The main application remains a limited demonstration flow rather than a complete four-role user-facing LMS. This is a functionality/design gap, not a static-analyser finding.

These are tracked as review risks in `PHASE6_CODE_REVIEW_RECORD.md`. They require code changes and tests before being marked resolved.
