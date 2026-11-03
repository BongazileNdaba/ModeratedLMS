# Moderated LMS â€“ Release Notes v1.1 (Phase 8 candidate)

## Change request CR1
- Added whole-class assignment deadline extensions, recorded with the responsible lecturer, timestamp and reason.
- Student-specific effective deadlines now resolve from the latest applicable individual or class-wide extension.
- Added a student assignment-list service query that returns original and effective deadlines and flags extended deadlines.
- Added regression tests for class-wide deadline visibility in the returned assignment list, individual-extension isolation and student-role authorization.

## Compatibility and data
The class-extension table is created additively by `WorkflowService`, preserving existing assignment and submission records. Existing v1.0 data is not intentionally rewritten.

## Verification
This is a **release candidate/documentation package**, not evidence of a verified production release. Build and full automated regression results must be captured on the target Windows/MSYS2 environment. Manual lecturer/student checks are still required.

## Known limitations
The interactive student assignment-list screen is not implemented; the service now returns the data needed to render it. The project remains a backend/service prototype and has not demonstrated multi-workstation networking or 800-student load performance. Do not tag as v1.1 until outstanding verification is complete.
