# User Manual â€” System Administrator

**Role:** Administrator
**System:** Moderated LMS
**Release target:** v1.0

## 1. Role and responsibilities

The Administrator maintains the school's core records and user accounts. The intended responsibilities are to create and maintain departments, courses and accounts; assign lecturers to courses; and enrol students in the appropriate courses. The Administrator should not alter marks or bypass the HoD moderation process.

## 2. Access and security

- Use the account issued by the school's authorised system administrator.
- Keep credentials private and use a unique password.
- Sign out or secure the workstation when leaving it unattended.
- Only create accounts for verified staff and students.
- Assign the minimum role and department/course access needed.
- Disable accounts when a user leaves or no longer requires access; do not delete assessment history without an approved retention procedure.

## 3. Intended operating procedures

The following procedures describe the required role workflow. The current archive does not yet provide complete interactive Administrator menus; these are operational requirements for the finished interface, not claims that every screen exists today.

### 3.1 Create a department
1. Open the Administrator area and choose **Departments**.
2. Select **Add department**.
3. Enter the official department name and any required code.
4. Review the details and save.
5. Confirm the department appears in the department list.

### 3.2 Create a course
1. Choose **Courses** then **Add course**.
2. Enter the course code, title and owning department.
3. Validate that the course code is unique.
4. Save and confirm the course is listed under the correct department.

### 3.3 Create user accounts
1. Choose **User Accounts** then **Create account**.
2. Enter the approved username, temporary password and role.
3. For departmental staff, select the correct department where applicable.
4. Save the account and provide the initial credentials securely.
5. Require the user to change a temporary password if that feature is provided by the final release.

Passwords must never be stored or exported in plaintext. The current backend uses libsodium Argon2id hashing with a random salt.

### 3.4 Assign a lecturer and enrol students
1. Open the relevant course.
2. Assign the authorised lecturer.
3. Search for each student using their verified account identifier.
4. Add the student to the course enrolment list.
5. Review the enrolment list for incorrect or duplicate entries.

### 3.5 Disable an account
1. Locate the account.
2. Confirm the identity and reason for deactivation.
3. Choose **Deactivate** and confirm.
4. Verify that the account can no longer authenticate.
5. Retain the audit/administrative record according to school policy.

## 4. Common problems

- **Duplicate username/course code:** check for an existing record before retrying; identifiers should be unique.
- **Incorrect department/course assignment:** correct the record through the authorised administrative workflow and record the reason.
- **User cannot log in:** verify username, active status and role; never ask the user to send a password in plain text.

## 5. Current release limitation

In the supplied v1.0-target archive, `main.cpp` demonstrates account creation and authentication, but complete Administrator menus for department/course management, lecturer assignment and student enrolment are not yet implemented as an interactive workflow. This manual defines the intended use and must be reconciled with the final interface before production issue.
