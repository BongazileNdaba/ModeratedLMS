# Phase 8 Deliverables — Maintenance (CR1)

**Handout deliverable:** change-request analysis, v1.1 release and regression-test results.

## Change request
CR1: a lecturer can extend an assignment deadline for a whole class, and the extension appears on each affected student's assignment list.

## Included evidence
- `PHASE8_CHANGE_REQUEST_CR1.md`: maintenance classification and impact analysis.
- `PHASE8_RELEASE_NOTES_v1.1.md`: intended v1.1 changes.
- `PHASE8_REGRESSION_STATUS.md`: regression-test status.
- `../src/services/WorkflowService.cpp`: whole-class extension persistence and effective-deadline logic.
- `../include/services/WorkflowService.h`: service API and student assignment data transfer object.
- `../tests/WorkflowServiceTests.cpp`: automated regression coverage for workflow rules.

## Acceptance evidence still to record
Run the complete test suite against the final v1.1 candidate, including the pre-existing tests and CR1-specific tests. Execute a manual test with at least two enrolled students and verify that both see the class extension while an unrelated student/course is unaffected. Record the actual output and test date. Update affected design artefacts and traceability before creating the v1.1 release tag.
