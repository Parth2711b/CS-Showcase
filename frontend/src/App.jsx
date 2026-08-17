import React, { useState, useEffect, useRef } from 'react';

class ErrorBoundary extends React.Component {
  constructor(props) {
    super(props);
    this.state = { hasError: false, error: null };
  }
  static getDerivedStateFromError(error) { return { hasError: true, error }; }
  render() {
    if (this.state.hasError) return <div style={{ color: 'red', padding: '20px' }}><h2>React Error:</h2><pre>{this.state.error.toString()}</pre></div>;
    return this.props.children;
  }
}

function App() {
  const [status, setStatus] = useState("Disconnected");
  const [socket, setSocket] = useState(null);
  const [inputMsg, setInputMsg] = useState("");
  const [forceRollback, setForceRollback] = useState(false);
  
  const [messages, setMessages] = useState([]);
  const [osLogs, setOsLogs] = useState([]);
  const [dbLogs, setDbLogs] = useState([]);
  const [activeThreads, setActiveThreads] = useState(new Set());
  
  const messagesEndRef = useRef(null);

  useEffect(() => { messagesEndRef.current?.scrollIntoView({ behavior: "smooth" }); }, [messages]);

  useEffect(() => {
    let ws;
    let reconnectTimer;
    let isMounted = true; 

    const connect = () => {
      ws = new WebSocket('ws://localhost:8080');

      ws.onopen = () => {
        if (!isMounted) return ws.close();
        setStatus("Connected");
        setSocket(ws);
      };

      ws.onmessage = (event) => {
        if (!isMounted) return;
        try {
          const data = JSON.parse(event.data);
          const time = new Date().toLocaleTimeString();

          if (data.type === 'history') {
            // Load history array from DB
            setMessages(data.messages);
          }
          else if (data.type === 'chat') {
            setMessages((prev) => [...prev, { sender: data.sender, text: data.text }]);
          } 
          else if (data.type === 'db_event') {
            const logEntry = `[${time}] ${data.event}`;
            setDbLogs((prev) => [...prev, logEntry]);
          }
          else if (data.type === 'os_event') {
            const logEntry = `[${time}] [Thread ${data.thread_id}] ${data.event}`;
            setOsLogs((prev) => [...prev, logEntry]);

            if (data.event === "Thread Spawned") {
               setActiveThreads(prev => {
                  const newSet = new Set(prev);
                  newSet.add(data.thread_id);
                  return newSet;
               });
            } else if (data.event === "Thread Terminated") {
               setActiveThreads(prev => {
                  const newSet = new Set(prev);
                  newSet.delete(data.thread_id);
                  return newSet;
               });
            }
          }
        } catch (e) {
          setMessages((prev) => [...prev, { sender: 'Server (Raw)', text: event.data }]);
        }
      };

      ws.onerror = () => { if (isMounted) setStatus("Reconnecting..."); };

      ws.onclose = () => {
        if (!isMounted) return;
        setStatus("Disconnected");
        setSocket(null);
        reconnectTimer = setTimeout(connect, 2000);
      };
    };

    connect();

    return () => {
      isMounted = false;
      clearTimeout(reconnectTimer);
      if (ws) { ws.onmessage = null; ws.close(); }
    };
  }, []);

  const sendMessage = () => {
    if (socket && inputMsg !== "") {
      const payload = forceRollback ? "/rollback " + inputMsg : inputMsg;
      socket.send(payload);
      setMessages(prev => [...prev, { sender: 'You', text: inputMsg }]);
      setInputMsg(""); 
    }
  };

  return (
    <div style={{ padding: '20px', fontFamily: 'sans-serif' }}>
      <h2>Multithreaded TCP Server Test</h2>
      <p>Connection: <strong>{status}</strong> | Active Threads: <strong>{activeThreads.size}</strong></p>
      
      <div style={{ display: 'flex', gap: '20px', marginTop: '20px' }}>
        
        {/* Chat Section */}
        <div style={{ flex: 1, border: '4px solid black', padding: '10px', display: 'flex', flexDirection: 'column' }}>
          <h3>Chat Client</h3>
          
          {/* Main Chat Box */}
          <div style={{ flex: 1, height: '250px', overflowY: 'scroll', border: '3px solid gray', marginBottom: '10px', padding: '10px' }}>
            <strong>Live Chat (Server Broadcasts):</strong><br/><br/>
            {messages.filter(m => m.sender !== 'You').length === 0 ? "No broadcasts received yet." : ""}
            {messages.filter(m => m.sender !== 'You').map((msg, idx) => (
              <div key={idx} style={{ marginBottom: '5px' }}>
                <strong>{msg.sender}:</strong> {msg.text}
              </div>
            ))}
            <div ref={messagesEndRef} />
          </div>
          
          {/* Input Area */}
          <div style={{ marginBottom: '10px' }}>
            <input 
              type="text" 
              value={inputMsg}
              onChange={(e) => setInputMsg(e.target.value)}
              onKeyDown={(e) => e.key === 'Enter' && sendMessage()}
              style={{ width: '80%', padding: '5px' }}
            />
            <button onClick={sendMessage} style={{ padding: '5px 10px', marginLeft: '5px' }}>Send</button>
            <div style={{ marginTop: '10px' }}>
              <label style={{ fontSize: '0.9rem', cursor: 'pointer' }}>
                <input 
                  type="checkbox" 
                  checked={forceRollback} 
                  onChange={(e) => setForceRollback(e.target.checked)} 
                  style={{ marginRight: '5px' }}
                />
                Force Transaction Failure (Demo Rollback)
              </label>
            </div>
          </div>

          {/* Sent Messages Box */}
          <div style={{ height: '100px', overflowY: 'scroll', border: '3px solid gray', padding: '10px' }}>
            <strong>Raw Sent Data (What you typed):</strong><br/><br/>
            {messages.filter(m => m.sender === 'You').length === 0 ? "No messages sent." : ""}
            {messages.filter(m => m.sender === 'You').map((msg, idx) => (
              <div key={idx} style={{ marginBottom: '5px' }}>
                <strong>{msg.sender}:</strong> {msg.text}
              </div>
            ))}
          </div>
        </div>

        {/* Logs Section */}
        <div style={{ flex: 1, border: '4px solid black', padding: '10px', display: 'flex', flexDirection: 'column' }}>
          <h3>Backend OS Logs</h3>
          
          {/* Explanation Legend */}
          <div style={{ backgroundColor: '#f9f9f9', border: '1px solid #ccc', padding: '10px', marginBottom: '10px', fontSize: '0.85rem', color: '#333' }}>
            <strong>Legend:</strong>
            <ul style={{ margin: '5px 0 0 0', paddingLeft: '20px' }}>
              <li><strong>Thread Spawning:</strong> Shows when the server allocates a new std::thread for an incoming connection.</li>
              <li><strong>Mutex Locking:</strong> Shows when std::lock_guard acquires the clients_mutex to prevent race conditions during message broadcast.</li>
            </ul>
          </div>

          <div style={{ flex: 1, height: '300px', overflowY: 'scroll', border: '3px solid gray', padding: '10px', fontFamily: 'monospace' }}>
            {osLogs.length === 0 ? "No logs yet." : ""}
            {osLogs.map((log, i) => (
              <div key={i} style={{ marginBottom: '5px' }}>{log}</div>
            ))}
          </div>
        </div>

        {/* DBMS Logs Section */}
        <div style={{ flex: 1, border: '4px solid black', padding: '10px', display: 'flex', flexDirection: 'column' }}>
          <h3>DBMS Logs (SQLite)</h3>
          
          {/* Explanation Legend */}
          <div style={{ backgroundColor: '#e9f5e9', border: '1px solid #ccc', padding: '10px', marginBottom: '10px', fontSize: '0.85rem', color: '#333' }}>
            <strong>Legend:</strong>
            <ul style={{ margin: '5px 0 0 0', paddingLeft: '20px' }}>
              <li><strong>ACID Properties:</strong> Transactions are wrapped in BEGIN and COMMIT blocks.</li>
              <li><strong>Persistence:</strong> Chat messages are securely INSERTed into test.db before broadcasting.</li>
            </ul>
          </div>

          <div style={{ flex: 1, height: '300px', overflowY: 'scroll', border: '3px solid gray', padding: '10px', fontFamily: 'monospace' }}>
            {dbLogs.length === 0 ? "No database transactions yet." : ""}
            {dbLogs.map((log, i) => (
              <div key={i} style={{ marginBottom: '5px', color: log.includes('COMMIT') ? 'green' : (log.includes('BEGIN') ? 'blue' : 'black') }}>{log}</div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
}

export default function AppWithErrorBoundary() {
  return <ErrorBoundary><App /></ErrorBoundary>;
}
