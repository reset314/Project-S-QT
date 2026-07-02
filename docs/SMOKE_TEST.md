# AI Chat Desktop — Smoke Test Checklist

This document describes the manual smoke test procedure for verifying that
the AI Chat Desktop client is functioning correctly after a build.

## Prerequisites

- A running instance of the AI Chat backend server (see `aaa/` project).
- At least one registered user account on the server.
- At least one AI user created on the server that the test user can chat with.
- The desktop client built from a clean checkout:
  ```bash
  mkdir build && cd build
  cmake .. && cmake --build .
  ```

---

## 1. Clean Build from Source

- [ ] Delete any existing `build/` directory.
- [ ] Run `cmake .. && cmake --build .` from a fresh `build/` directory.
- [ ] The build completes with zero errors and zero warnings (warnings may
      exist for third-party headers; focus on project source warnings).
- [ ] The `AIChatDesktop` executable is produced.

---

## 2. Application Launch

- [ ] Launch the application.
- [ ] The login page is displayed with username/password fields and a Login
      button.
- [ ] The window has a custom frameless title bar with minimize, maximize,
      and close buttons.
- [ ] The system tray icon appears (Windows/macOS/Linux).
- [ ] Right-clicking the tray icon shows a context menu with "Show / Hide"
      and "Quit".

---

## 3. Login Flow

- [ ] Enter valid credentials and click Login.
- [ ] The login button shows a loading spinner while the request is in flight.
- [ ] On success, the UI transitions from the login page to the main shell
      (sidebar + chat area).
- [ ] The sidebar shows the list of available AI users (contacts).
- [ ] The status bar shows a green "connected" indicator.
- [ ] **Negative test:** Enter invalid credentials. An error message is
      displayed. The UI remains on the login page.
- [ ] **Negative test:** Leave fields empty and click Login. The login is
      prevented or the server returns an appropriate error.

---

## 4. Token Persistence

- [ ] After successful login, close the application completely.
- [ ] Re-launch the application.
- [ ] The user should be automatically logged in (no login prompt).
- [ ] The token restore happens silently; if the access token is still valid,
      the user lands directly in the main shell.
- [ ] If the access token has expired but the refresh token is valid, a
      silent refresh occurs and the user is still logged in.

---

## 5. Send Text Message

- [ ] Click on an AI user in the sidebar to open a chat.
- [ ] Type a text message in the input field at the bottom.
- [ ] Press Enter or click the Send button.
- [ ] The message appears immediately as a user bubble (right-aligned, blue
      or theme-colored).
- [ ] A typing indicator (three animated dots) appears briefly on the AI side.
- [ ] The AI response streams in via WebSocket chunks and appears as an AI
      bubble (left-aligned).
- [ ] The AI message includes the response text; if the `think` section is
      present, it appears in a collapsible area.
- [ ] The message list auto-scrolls to the bottom as content arrives.

---

## 6. Streaming Response

- [ ] Verify that the AI response appears incrementally (word-by-word or
      chunk-by-chunk), not all at once.
- [ ] Verify that the `streamDone` frame marks the message as complete.
- [ ] Verify that the typing indicator disappears when streaming completes.
- [ ] **Interruption test:** While the AI is streaming a response, switch to
      a different AI user in the sidebar. The stream for the previous AI
      user should be cancelled gracefully (no crash, no orphaned timers).

---

## 7. Message History

- [ ] After sending several messages, close the application.
- [ ] Re-launch the application and open the same AI user chat.
- [ ] All previously sent and received messages are restored from the local
      SQLite database.
- [ ] The server sync runs on startup and reconciles any messages that
      arrived while the client was offline.

---

## 8. File Upload (Media Message)

- [ ] Open a chat with an AI user.
- [ ] Click the attachment button in the message input area.
- [ ] Select an image file from the local filesystem.
- [ ] The image is uploaded via multipart/form-data to the server.
- [ ] The message appears with a media preview thumbnail in the chat.
- [ ] The AI responds (text or media acknowledgement).

---

## 9. Unread Badge

- [ ] Log in as User A on one device and as User B on another device (or use
      the server API to send a message from a different account).
- [ ] Send a message from User B to an AI user that User A also has access to.
- [ ] On User A's client, verify that the unread badge on the sidebar item
      increments.
- [ ] Verify that the system tray icon changes to indicate unread messages.
- [ ] Verify that the taskbar/dock badge updates (logged via debug output in
      the stub implementation; native badge requires platform API
      integration).
- [ ] Click on the AI user's chat. The unread badge should clear and the
      tray icon should return to its normal state.

---

## 10. Active Chat Exclusion (UnreadTracker)

- [ ] Open a chat with AI User X.
- [ ] While viewing AI User X's chat, have another message arrive for AI
      User X (from another client or the server).
- [ ] The unread badge for AI User X should NOT increment (the user is
      actively viewing that chat).
- [ ] Switch to AI User Y. Have a message arrive for AI User X.
- [ ] The unread badge for AI User X SHOULD increment.

---

## 11. System Tray

- [ ] Click the window's close button. The window hides to the system tray
      instead of quitting.
- [ ] Double-click the tray icon. The window is restored and brought to the
      foreground.
- [ ] Right-click the tray icon and select "Show / Hide". The window toggles
      visibility.
- [ ] With the window hidden, press Ctrl+Shift+A. The window is restored.
- [ ] Right-click the tray icon and select "Quit". The application exits
      completely.

---

## 12. Message Sync & Stale Cleanup

- [ ] Send a message from the desktop client.
- [ ] Before the server confirms the message (SSE stream completes), kill the
      application.
- [ ] Wait 30 minutes (or manually advance the system clock for testing).
- [ ] Re-launch the application.
- [ ] Open the same AI user's chat.
- [ ] The stale unsynced message (no server_id, timestamp > 30 min old) is
      removed by the sync service's phantom cleanup.
- [ ] Messages that were confirmed by the server (have a server_id) are
      preserved.

---

## 13. Settings Persistence

- [ ] Open the Settings page.
- [ ] Change the theme (if a theme toggle is available).
- [ ] Close and re-launch the application.
- [ ] Verify the theme setting persists.
- [ ] Change the server URL in settings.
- [ ] Verify the new server URL is used for subsequent API calls.

---

## 14. Multi-Page Navigation

- [ ] Click "Chats" in the sidebar tab bar. The conversation list is displayed.
- [ ] Click "Contacts" in the sidebar tab bar. The contact list is displayed.
- [ ] Click on a contact to view their detail page.
- [ ] Navigate to Settings, Account, Profile, TTS Voices, and Expansion pages.
- [ ] Use the back button or sidebar to return to the chat view.
- [ ] No crashes, visual glitches, or blank pages occur during navigation.

---

## 15. Call System (if WebRTC is available)

- [ ] Open a chat with an AI user.
- [ ] Initiate a voice or video call.
- [ ] The CallPage window opens with the call UI.
- [ ] The call state transitions through idle -> ringing -> connecting ->
      active.
- [ ] Mute and camera toggle buttons work.
- [ ] Hang up ends the call and closes the CallPage.
- [ ] Incoming call dialog appears when receiving a call from another user.

---

## 16. Edge Cases and Error Handling

- [ ] **Network loss during message send:** Disconnect the network and try to
      send a message. An error is displayed, and the optimistic message
      remains in the local DB for retry.
- [ ] **Server down at startup:** Launch the app while the server is
      unreachable. The login page should still display; attempting to log in
      shows a network error.
- [ ] **Invalid server URL:** Configure a non-existent server URL. API calls
      should fail with descriptive network errors, not crash.
- [ ] **Large message history:** Send 50+ messages in a single chat. Scroll
      up to load earlier messages via pagination. Performance should remain
      acceptable.
- [ ] **Rapid message sending:** Send 5 messages in quick succession. The
      batching system should coalesce them without errors.
- [ ] **Special characters:** Send messages containing emojis, Chinese
      characters, and JSON-like text. All should display correctly.

---

## Test Environment

Record the environment used for each smoke test run:

| Item | Value |
|------|-------|
| Date | |
| Tester | |
| OS | |
| Qt version | |
| Server version | |
| Build type | Debug / Release |
| Passed | Yes / No |
| Notes | |
