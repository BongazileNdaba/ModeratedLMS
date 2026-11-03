# CS4433 Final Project — Handout Deliverables Index

This index follows the final individual assignment handout. It is an evidence index, not a declaration that every test, CI run, release tag or deployment activity has already been executed.

| Handout phase | Required deliverable | Project evidence | Evidence status |
|---|---|---|---|
| Phase 5 — Verification and Validation | Test plan, test code, test results, coverage report, defect log | `PHASE5_DELIVERABLES.md`, `PHASE5_TEST_PLAN.md`, `PHASE5_TRACEABILITY_MATRIX.md`, `tests/`, `system_tests/PHASE5_SYSTEM_ACCEPTANCE_TESTS.md`, `PHASE5_EXECUTION_STATUS.md`, `DEFECT_LOG.md` | Automated tests exist; exact archive must be rerun; manual results and fresh coverage evidence pending |
| Phase 6 — Configuration Management and QA | Repository link, CI evidence, static analysis, metrics, code review | `PHASE6_DELIVERABLES.md`, `PHASE6_GIT_WORKFLOW.md`, `.github/workflows/ci.yml`, `PHASE6_STATIC_ANALYSIS_REPORT.md`, `PHASE6_CODE_METRICS.md`, `PHASE6_CODE_REVIEW_RECORD.md` | Workflow configured; hosted run, actual repository evidence and refreshed QA outputs pending |
| Phase 7 — Deployment | Deployment diagram, installation guide, role manuals, v1.0 release notes/tag | `PHASE7_DELIVERABLES.md`, deployment diagram files, installation guide, four role manuals, release notes | Documents present; reconcile manuals with final executable and verify actual release tag |
| Phase 8 — Maintenance | CR1 analysis, v1.1 implementation, regression test results | `PHASE8_DELIVERABLES.md`, CR1 analysis, v1.1 notes, regression status, workflow service and tests | CR1 implementation/test code present; rerun and manual regression evidence pending |

## Submission-wide requirements from the handout

The final submission is a single 6,000–8,000-word PDF report (diagrams, tables and appendices excluded) plus a Git repository link. The report must include: title page/abstract/contents; introduction; project management; requirements engineering; software design; implementation; verification and validation; configuration management and QA; deployment; maintenance; professional/legal/ethical issues; conclusion; references; and appendices containing traceability, full test results, role manuals and updated risk register.

The handout also requires a 15–20 minute viva. The student must be able to explain their own code and design decisions. This cannot be completed by preparing files alone.

## Release safety rule
Do not overwrite the currently working local project or the existing GitHub branch until the candidate has been built and tested. Do not claim a Windows build, passing CI run, static-analysis-clean result, 800-student performance result, production deployment, or release tag unless the corresponding evidence has actually been produced.
