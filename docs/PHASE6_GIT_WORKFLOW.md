# Phase 6 â€” Git Configuration Management Workflow

## Repository layout and protected branch

- `main`: stable integration branch; protect it from direct pushes where repository settings allow.
- `feature/<short-topic>`: new functionality, tests or documentation.
- `fix/<short-topic>`: defect correction.
- `docs/<short-topic>`: documentation-only changes.

## Standard change process

1. Update local `main`: `git switch main` then `git pull --ff-only`.
2. Create a focused branch, for example `git switch -c feature/password-hasher-tests`.
3. Make one logically related change and update/add tests and documentation.
4. Inspect the diff: `git diff --check` and `git diff`.
5. Configure/build/test locally: `cmake -S . -B build -DBUILD_TESTING=ON`, `cmake --build build`, `ctest --test-dir build --output-on-failure`.
6. Run quality checks: `cppcheck ...` and `python scripts/collect_metrics.py`.
7. Commit with a meaningful message and push the branch.
8. Open a pull request to `main`; describe the change, tests, risks and screenshots/logs if relevant.
9. Merge only after review and required CI checks pass; delete the merged feature branch.

## Commit message convention

Use `type(scope): imperative summary`, with a concise body explaining why and any compatibility impact.

Examples for future work (these are examples, not claims that these commits already exist):

- `test(auth): cover inactive and invalid login attempts`
- `test(security): verify Argon2id hash and salt behaviour`
- `ci(cmake): register all GoogleTest suites and coverage runtime`
- `feat(workflow): support whole-class deadline extensions`
- `docs(phase6): add QA evidence and review record`

Allowed types: `feat`, `fix`, `test`, `docs`, `refactor`, `build`, `ci`, `chore`.

## Release and tag procedure

Only tag a release after the corresponding commit has passed CI and the release notes are updated. Use annotated, immutable tags:

```bash
git switch main
git pull --ff-only
git tag -a v1.0.0 -m "Moderated LMS initial release"
git push origin v1.0.0
```

For the Phase 8 change release, after validating the change and its regression suite:

```bash
git tag -a v1.1.0 -m "Moderated LMS whole-class extension update"
git push origin v1.1.0
```

Do not run these commands against the current ZIP snapshot until the actual repository history and release state have been checked. If v1.0.0 or v1.1.0 already exists, inspect it rather than overwriting it.

## Repository evidence to attach

- Hosted repository URL.
- Screenshot/export showing branch list and pull request(s).
- Commit history showing meaningful messages.
- `git tag --list` output and tag details (`git show v1.1.0`).
- GitHub Actions run URL for the submitted commit.

The uploaded project ZIP contains no `.git` directory, so these items must be captured from the user's hosted repository; they cannot be recreated honestly from the ZIP alone.
