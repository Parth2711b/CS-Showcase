# Multithreaded TCP WebSocket Server & OS Visualizer

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![React](https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Windows Sockets](https://img.shields.io/badge/Winsock2-4D4D4D?style=for-the-badge&logo=windows&logoColor=white)

This project is a **Multi-Threaded Chat Server** built using C++ (Backend) and React (Frontend). The main objective is not just to build a chat application, but to build core Computer Science concepts (OS, DBMS, Networking) from scratch to understand them deeply.

## Tech Stack
- **Backend:** Modern C++20, CMake, Raw Windows Sockets (Winsock2)
- **Cryptography:** Custom SHA-1 & Base64 encoding (for WebSocket Handshake)
- **Frontend:** React.js, Native WebSockets, Vite
- **Data Format:** JSON over raw TCP frames

## Core CS Concepts Implemented

### 1. Operating Systems (Concurrency & Synchronization)
- **Multithreading (`std::thread`):** Unlike a single-threaded loop that blocks other users, this server spawns a dedicated OS-level worker thread for every new client connection. (Visible live on the frontend as *Thread Spawned*).
- **Concurrency & Race Conditions (`std::mutex`):** When multiple clients send messages simultaneously, they try to write to the socket at the exact same millisecond. To prevent data corruption, a Mutual Exclusion lock (`std::lock_guard`) is used to queue the broadcasting safely.
- **Memory Management & Cleanup:** When a client disconnects unexpectedly, the OS traps the `recv()` failure and gracefully terminates the specific thread, freeing up RAM. (Visible as *Thread Terminated*).

### 2. Computer Networks (CN)
- **TCP vs UDP:** TCP creates a "stream" where data order and delivery are guaranteed.
- **WebSockets from Scratch:** Handling the WebSocket handshake over raw TCP, implementing custom 0x81 Opcode Framing, and manual bitwise XOR unmasking.
- **Custom Protocol:** Designed the structure for JSON-based payloads (`chat`, `os_event`).

## Architecture & Design Decisions
- **The Handshake Process:** The browser sends a normal HTTP request with a secret `Sec-WebSocket-Key`. The C++ Server appends a Magic GUID `258EAFA5-E914-47DA-95CA-C5AB0DC85B11` to it. The combined string is hashed via `SHA-1` and encoded in `Base64`. The final response is sent as `HTTP/1.1 101 Switching Protocols`.
- **Security & Hacking Prevention:** This complicated math prevents non-WebSocket servers (like a Router or Printer) from accidentally connecting.
- **OOPs Architecture:** Smart use of C++ Inheritance. `TCPServer` (Parent) handles basic sockets (bind, listen, accept), and `WebSocketServer` (Child) uses the parent's work to focus solely on the new rules (Handshake/Parsing).

## OS Kernel Visualizer (Frontend Integration)
- **The Goal:** Visualize invisible OS events (Threads and Mutex) running on the backend live on the frontend to provide physical evidence of multithreading.
- **UI Design:** Intentionally designed a "raw" terminal-style UI focusing purely on the backend concepts.

## Bugs and Learnings
- **The Zombie WebSocket Leak (Memory Leak):** React Strict Mode rapidly mounted and unmounted components, but because `ws.close()` is asynchronous, a "Zombie Connection" reconnected in the background. This caused single messages to broadcast twice. Fixed by implementing an `isMounted` boolean flag inside the `useEffect` cleanup.
- **Live Race Condition Avoidance:** The frontend monitor shows that when two browsers send a message simultaneously, the C++ server instantly activates `std::lock_guard<std::mutex>`. The UI "Mutex Locked" event proves the `broadcastMessage()` function is 100% thread-safe.
- **Thread Lifecycle (Spawning & Cleanup):** The 'Active Threads' counter on the frontend proves that opening a new browser tab actually allocates a new `std::thread` in memory. Closing the tab causes `recv()` to fail, terminating the thread and ensuring no RAM is wasted.
- **Byte Order (`htons`):** Computers store bytes in different orders (endianness). Used `htons()` (Host to Network Short) to convert the host format to the standard network format (Big Endian) so values are interpreted correctly.
