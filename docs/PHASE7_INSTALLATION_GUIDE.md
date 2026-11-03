# Phase 7 â€” Installation and Deployment Guide

**System:** Moderated Learning Management System (Moderated LMS)
**Target release:** v1.0
**Platform documented:** Windows 10/11, C++17, SQLite, libsodium
**Audience:** School IT technician / system administrator

## 1. Purpose and deployment status

This guide explains how to prepare a Windows school computer, build the current source archive, initialise the local database and perform basic verification. The current candidate includes an interactive console menu for Administrator, Lecturer, HoD and Student roles, backed by authentication and workflow services. It is suitable for controlled demonstration and acceptance testing, but it is not yet approved for production school-wide use. Submission handling currently records a file path rather than managing a secure copied upload, and the application does not implement a remote network client/server protocol. Do not roll it out to all 800 students until the outstanding items in Section 10 are completed and accepted.

## 2. Target environment

### Hardware recommendation

- One dedicated school computer acting as the application/database host.
- Modern multi-core CPU, at least 8 GB RAM recommended for development/test; validate actual needs with a load test before school deployment.
- SSD storage with sufficient capacity and routine backup space.
- Reliable power and a UPS where available.
- If the final system is extended to multiple terminals, a managed wired LAN and access controls are required. The present executable does not implement a network client/server protocol.

### Software requirements

- Windows 10/11 64-bit.
- MSYS2 UCRT64 environment and GCC C++17 toolchain (or a separately validated MSVC configuration).
- CMake 3.20 or later.
- GNU Make for the documented MinGW Makefiles generator.
- SQLite development package.
- libsodium development package and runtime DLL available on PATH.
- Internet access for the first test-enabled configure if GoogleTest has not already been fetched. Internet is not needed for normal offline runtime after dependencies and build artefacts are installed.

## 3. Prepare the host (Windows / MSYS2 UCRT64)

1. Sign in using a Windows account with permission to install software. Do not run the LMS daily as an administrator.
2. Install MSYS2 from its official distribution and open **MSYS2 UCRT64**.
3. Update the package database and installed packages:

   ```bash
   pacman -Syu
   ```

   If the terminal closes or requests a restart, reopen UCRT64 and repeat the update until complete.

4. Install required packages:

   ```bash
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-sqlite3 mingw-w64-ucrt-x86_64-libsodium
   ```

5. Confirm the tools in the UCRT64 terminal:

   ```bash
   g++ --version
   cmake --version
   mingw32-make --version
   ```

6. Add `C:\msys64\ucrt64\bin` to the Windows PATH if the build is to be run from PowerShell or VS Code. Reopen the terminal after changing PATH.

## 4. Copy and inspect the project

1. Copy the supplied project ZIP to a controlled folder, for example `C:\SchoolSystems\`.
2. Extract it so the project root contains `CMakeLists.txt`, `main.cpp`, `include`, `src`, `tests` and `docs`.
3. Keep the original ZIP as an immutable backup. Do not extract over an existing live database or source tree.
4. Open PowerShell in the project root and check:

   ```powershell
   Get-ChildItem
   ```

5. Confirm the required dependency headers and libraries exist under `C:\msys64\ucrt64\include` and `C:\msys64\ucrt64\lib`.

## 5. Configure, build and test

Run from the project root in PowerShell:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected outcome: CMake configures, all targets compile and CTest reports the registered test suites passing. Use the actual terminal output as the evidence; do not copy an expected result into the deployment record if the command fails.

For an application-only build on a machine without internet access (and where GoogleTest has not already been cached):

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DBUILD_TESTING=OFF
cmake --build build
```

This application-only configuration does not execute automated tests. Retain test evidence from a separate development/CI machine.

## 6. First launch and database initialisation

1. From the project root, run:

   ```powershell
   .\build\ModeratedLMS.exe
   ```

2. The program initialises the SQLite database file `moderated_lms.db` in its current working directory. On a fresh database, it prompts for the initial Administrator account; after that, users log in and receive the menu corresponding to their registered role.
3. Confirm that the process exits successfully and that the database file is created.
4. Keep the database in a directory with restricted Windows permissions. The database contains personal and assessment-related records; do not place it in a publicly shared folder.
5. Do not use real student data during prototype verification. Use synthetic test accounts until the system passes security, privacy and acceptance review.

## 7. Backup and recovery procedure

The school IT technician should:

1. Stop the application before copying the database, or use a SQLite-consistent backup method.
2. Copy `moderated_lms.db` to a restricted backup directory.
3. Name backups with date and time, for example `moderated_lms_YYYYMMDD_HHMM.db`.
4. Restrict access to authorised IT personnel; encrypt backups at rest and protect removable media.
5. Retain multiple generations according to the school's approved retention schedule.
6. Test restoration periodically on a separate test copy. Never test recovery by overwriting the only live database.

A backup is not verified until a restore test has succeeded.

## 8. Updating the application

1. Schedule maintenance and notify users.
2. Stop the application and create a verified database backup.
3. Preserve the current executable, source version and database backup.
4. Obtain the approved release archive and verify its source/release tag.
5. Build and run the test suite in a staging folder first.
6. Apply any documented database migration only after reviewing its compatibility and backup/rollback plan.
7. Replace the application binary only after staging acceptance.
8. Record version, date, operator, test result and rollback location.

Do not assume a source update automatically migrates every production database safely.

## 9. Troubleshooting

| Symptom | Checks / action |
|---|---|
| `cmake` not recognised | Confirm CMake is installed and its bin directory is on PATH; reopen terminal. |
| `g++` or `mingw32-make` not recognised | Use MSYS2 UCRT64 or add `C:\msys64\ucrt64\bin` to PATH. |
| `sodium.h` not found | Install the UCRT64 libsodium development package and verify include path. |
| SQLite library not found | Install UCRT64 SQLite development package and verify CMake can locate it. |
| GoogleTest download fails | Configure with internet or use a machine with cached GoogleTest; application-only build can use `BUILD_TESTING=OFF`. |
| DLL missing at launch | Ensure the required UCRT64 runtime DLLs are installed and available on PATH. |
| Database initialisation fails | Check folder permissions, disk space, file lock and `Database::lastError()` output. Preserve the database before attempting repair. |

## 10. Deployment acceptance checklist and known limitations

The following must be completed before production deployment:

- [ ] Complete role-specific interactive workflows and access-control checks at the application boundary.
- [ ] Implement and validate student assignment lists and released-mark/feedback views.
- [ ] Implement actual submission file validation, safe storage, size/type limits and retention policy; current `submit()` stores a path and timestamp rather than copying/validating file contents.
- [ ] Implement/validate a supported multi-user or local-network access model if multiple workstations are required. Current application is not a network server.
- [ ] Run performance and concurrency tests against the 800-student requirement on the school's actual hardware.
- [ ] Complete manual system and acceptance tests with representative users.
- [ ] Complete security review, data-protection review, access-control review and backup/restore test.
- [ ] Confirm that all user manuals match the final implemented interface.
- [ ] Obtain school sign-off and approve a production release tag.

**Deployment decision:** suitable for controlled development/testing and demonstration of the current backend prototype; **not approved for production school-wide deployment** until all checklist items are closed.
