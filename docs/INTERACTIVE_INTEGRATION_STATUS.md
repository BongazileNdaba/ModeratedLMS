# Interactive integration status

## Purpose
Restore the interactive role-based console application while retaining the current Phase 5–8 service layer and automated tests as the implementation foundation.

## Implemented in this integration
- First-run bootstrap for the initial Administrator account.
- Login through `AuthenticationService` and account persistence through `UserRepository`.
- Administrator menus for account creation/listing/status, departments, courses and student enrolment.
- Lecturer menus for assignment creation, individual/class extensions, current-submission listing and marking.
- HoD menus for moderation decisions, change requests and separate mark release.
- Student menus for assignment/effective-deadline viewing, submission/resubmission, submission history and released results.
- Assignment, extension, submission, mark, moderation and release actions route through `WorkflowService`.
- Student file selection checks that a regular local file exists before recording the submission.

## Important implementation boundaries
- Submission records store a local file path; the application does not copy file bytes into managed storage or upload them to a server.
- This remains an offline, single-machine SQLite console application. It is not a networked multi-user client/server deployment.
- No 800-student load test is claimed by this integration.
- The prior snapshot's 36 passing tests predate this `main.cpp` change. Run the full build and CTest suite against this integration before calling it verified.
- Windows/MSYS2/libsodium build and end-to-end manual acceptance tests remain required.

## Manual acceptance walkthrough
1. Start with an empty database and create the initial Administrator.
2. Create a department, one Lecturer, one HoD and one Student, then create a course and enrol the Student.
3. Log in as Lecturer; create an assignment and grant both a class extension and an individual extension. Confirm the Student sees the applicable effective deadline.
4. Log in as Student; submit an existing local file, resubmit a second file, and inspect both versions in history.
5. Log in as Lecturer; inspect current submissions and mark the current version. Confirm the mark enters `AwaitingModeration` and the audit log records the reason.
6. Log in as HoD; request changes and confirm the mark is not visible to the Student. Log in as Lecturer and revise the mark.
7. Log in as HoD; approve the revised mark, then release it. Confirm the Student can now see the mark and feedback.
8. Repeat submissions at the deadline, 1–3 calendar days late, and more than 3 days late to verify the policy at the UI level.
