# User Manual â€” Lecturer

**Role:** Lecturer
**System:** Moderated LMS
**Release target:** v1.1 candidate

## 1. Role and responsibilities

A Lecturer creates assignments for courses they teach, sets deadlines and maximum marks, grants individual extensions (and, in v1.1, whole-class extensions), reviews submissions, records marks and written feedback, and sends marks to the Head of Department for moderation. A lecturer must not release marks directly to students before HoD approval.

## 2. Intended operating procedures

The current project archive implements key workflow operations in `WorkflowService`, but does not yet include a complete interactive Lecturer menu. The steps below describe the target user workflow.

### 2.1 Create an assignment
1. Sign in using your Lecturer account.
2. Select a course assigned to you.
3. Choose **Create assignment**.
4. Enter a clear title/instruction, deadline date/time and maximum mark.
5. Check the deadline and maximum mark carefully before saving.
6. Confirm the assignment appears under the correct course.

### 2.2 Grant an individual extension
1. Open the assignment and select the student.
2. Confirm the student's request and the reason/evidence according to school policy.
3. Enter the revised effective deadline.
4. Save the extension and verify it applies only to the selected student.
5. Communicate the approved deadline to the student.

### 2.3 Grant a whole-class extension (Phase 8 / v1.1)
1. Open the assignment and choose **Extend for class**.
2. Confirm the course/class and assignment.
3. Enter the revised deadline and reason.
4. Review the scope carefully; this must apply to the intended class, not unrelated courses.
5. Save and verify that students' assignment lists display the revised deadline in the final interface.

The backend contains whole-class extension functionality and a student assignment-list query that returns the revised effective deadline. The interactive student-facing list screen is still not implemented, so do not promise that students can currently see the change in a running user interface.

### 2.4 Review and mark submissions
1. Open the assignment's submission list.
2. Review the current submission version and submission timestamp.
3. Assess the work using the approved marking criteria.
4. Enter a raw mark within the allowed range and written feedback.
5. Submit the mark for moderation.
6. If a mark correction is needed, provide a reason so the change can be audited.

### 2.5 Understand late penalties
The configured policy is based on calendar days and the effective deadline (including an approved extension):

- Submitted on or before the effective deadline: no late deduction.
- 1 day late: deduct 10% of the maximum mark.
- 2 days late: deduct 20% of the maximum mark.
- 3 days late: deduct 30% of the maximum mark.
- More than 3 days late: accept and record the submission, but the mark is zero.

The penalty is calculated from the assignment's maximum mark, not 10% of the student's achieved raw mark. Confirm the calculated result before sending it for moderation.

### 2.6 Respond to HoD moderation
- **Approved:** the HoD has accepted the mark; the system may then release it according to the authorised release process.
- **Changes requested:** review the HoD's comments, correct the mark/feedback if appropriate, record the reason and resubmit for moderation.

## 3. Good practice

- Mark consistently against published criteria.
- Keep feedback constructive and relevant.
- Do not share student submissions or marks with unauthorised people.
- Never bypass moderation or edit the database directly.
- Preserve submission versions and explain mark changes.

## 4. Current release limitation

The current v1.0-target archive has backend workflow methods for assignment creation, extensions, submission versioning, marking and moderation. It does not yet provide a complete interactive Lecturer interface, nor actual file-content upload/validation. `submit()` records a file path and timestamp; it does not copy or inspect the submitted file. These limitations must be resolved before operational use.
