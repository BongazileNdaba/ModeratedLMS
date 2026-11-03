# User Manual â€” Student

**Role:** Student
**System:** Moderated LMS
**Release target:** v1.1 candidate

## 1. Role and responsibilities

Students use the LMS to access assignments for courses in which they are enrolled, submit work, correct mistakes by resubmitting within the permitted period, and view marks and written feedback after the Head of Department has approved and released them.

## 2. Intended operating procedures

The archive now provides a backend assignment-list query (`WorkflowService::listAssignmentsForStudent`) that returns the original deadline, effective deadline and an extension indicator for each assignment in the studentâ€™s enrolled courses. The interactive Student menu/screen is still not implemented; the steps below describe the intended interface workflow.

### 2.1 View assignments
1. Sign in using your Student account.
2. Open **My Courses**.
3. Select the relevant course.
4. Open **Assignments**.
5. Check the instructions, maximum mark and deadline.
6. If an extension has been approved, use the revised effective deadline shown by the system. If the displayed date appears wrong, contact your lecturer promptly.

### 2.2 Submit an assignment
1. Open the correct assignment.
2. Check that the file is the final version you intend to submit.
3. Choose **Submit assignment** and select the file.
4. Confirm the selected filename and submit.
5. Record the confirmation/reference and submission time.
6. If the system reports an error, do not assume the submission succeeded; retry or contact the lecturer/IT support.

### 2.3 Resubmit work
1. Open the assignment before its effective deadline.
2. Select **Resubmit**.
3. Choose the corrected file and confirm.
4. Check that the latest version is identified as the current submission.
5. Keep your own copy of each submitted version.

The intended system preserves submission versions. The current backend stores a file path and timestamp; actual file upload, content storage and file validation are not implemented in the supplied archive.

### 2.4 Understand late submissions
- The effective deadline date is not late.
- Work up to three calendar days late is accepted with a penalty of 10% of the maximum mark per late day.
- Work more than three days late is still recorded but receives zero.
- An approved individual or class extension changes the effective deadline.

If you believe the recorded deadline or extension is incorrect, contact your lecturer as soon as possible and provide the relevant approval information.

### 2.5 View marks and feedback
1. Open **My Results** after the HoD has moderated and released the mark.
2. Select the course and assignment.
3. Review the released mark and lecturer feedback.
4. If you believe there is an error, follow the school's formal assessment query/appeal process.

Students must not be shown unreleased marks. The final interface must enforce this rule.

## 3. Student responsibilities and safety

- Submit your own work and follow school academic-integrity rules.
- Keep your login credentials private.
- Do not attempt to access another student's submissions or marks.
- Keep a backup of your assignment files and submission confirmations.
- Ask for help early if you have accessibility or technical difficulties.

## 4. Current release limitation

The backend assignment-list query is implemented, but the interactive Student-facing course/assignment screen, file-upload interface and released-mark/feedback screens remain outstanding. The revised deadline is available to a future screen through the service result; it is not yet displayed in a running UI. Do not use this prototype as the school's live submission channel until those features are completed and tested.
