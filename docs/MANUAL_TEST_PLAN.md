# Manual acceptance test plan

Use a disposable database, not real student data. Record pass/fail and screenshots for the submission report.

1. First run: confirm the application requests creation of the first Administrator; restart and confirm it does not ask again.
2. Login: correct credentials work; wrong password, unknown username and disabled account are rejected.
3. Administrator: create department, course, Lecturer, HoD and Student; list accounts; disable then re-enable an account; enrol the student.
4. Assignment: lecturer creates an assignment with a valid deadline and maximum mark; invalid course ID should not create it.
5. Extensions: create a class extension and an individual extension; student should see the individual deadline where both exist.
6. Submission: student records a file path; submit again and confirm version number increments and previous version is no longer current.
7. Marking: lecturer marks current submission; check raw mark, penalty and final mark in `marks`; check audit row in `mark_audit_log`.
8. Late policy: test deadline date (0 late days), 1 day late (10% of maximum mark deducted), 3 days late (30% deducted), and 4 days late (final mark zero). Repeat using individual and whole-class extensions.
9. Moderation: HoD approves a mark and student sees it; then test a separate mark where HoD requests changes and it is not shown as released.
10. Persistence: close/reopen app and confirm accounts, assignments, submissions and marks remain.
11. SQL/data safety: try duplicate usernames and invalid foreign keys; confirm errors do not crash the app.
12. Transaction failure: use an invalid operation where possible and verify partial submission/mark/moderation data is not left behind.

## Known testing gaps

These are acceptance tests to perform; they are not claimed as already passed. Full automated service-level tests, code coverage reporting, and validation of every role/ownership boundary remain to be added before production use.
