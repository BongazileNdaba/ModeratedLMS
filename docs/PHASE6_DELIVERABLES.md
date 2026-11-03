# Phase 6 Deliverables — Configuration Management and Quality Assurance

**Handout deliverable:** repository link, CI evidence, static analysis report, metrics and code review record.

## Included evidence
- `PHASE6_GIT_WORKFLOW.md`: branching, commit, review and release-tag procedure.
- `.github/workflows/ci.yml`: GitHub Actions workflow to build, run CTest, run cppcheck and upload service coverage on pushes and pull requests to `main`/`master`.
- `PHASE6_STATIC_ANALYSIS_REPORT.md`: static-analysis scope and findings record.
- `PHASE6_CODE_METRICS.md` and `../scripts/collect_metrics.py`: reproducible code-metrics method.
- `PHASE6_CODE_REVIEW_RECORD.md`: self-review checklist and change record.

## Evidence that must come from the hosted repository
A workflow file is not proof of a successful CI run. After pushing this exact revision, record the Actions run URL, commit SHA, date, build result, CTest summary, cppcheck result and coverage artifact. Record the actual repository URL and verify that the v1.0/v1.1 tags exist before claiming they are published. A ZIP cannot prove historical commits, pull requests or branch protection.

## Local QA status
The CI workflow is configured but has not been executed by GitHub in this preparation environment. Static-analysis and metrics reports must be regenerated against the final source revision. Record unresolved findings rather than claiming a clean scan without output.
