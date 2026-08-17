import React, { useState } from 'react';

/**
 * ChatUI Component
 * Demonstrates: Frontend User Interaction & State Management
 */
function ChatUI() {
  const [message, setMessage] = useState('');
  
  // TODO: Receive 'messages' array and 'sendMessage' function as props from App.jsx

  const handleSend = () => {
    // TODO: Send the message payload via WebSocket using the passed 'sendMessage' prop.
    setMessage('');
  };

  return (
    <div>
      <h2>Chat Interface</h2>
      <div style={{ height: '300px', border: '1px solid #ddd', overflowY: 'scroll', marginBottom: '10px' }}>
        {/* TODO: Map over 'messages' and display them here */}
        <p style={{ color: '#888', fontStyle: 'italic' }}>No messages yet...</p>
      </div>
      <div>
        <input 
          type="text" 
          value={message} 
          onChange={(e) => setMessage(e.target.value)} 
          placeholder="Type a message..."
          style={{ width: '80%', padding: '8px' }}
        />
        <button onClick={handleSend} style={{ padding: '8px 16px', marginLeft: '5px' }}>Send</button>
      </div>
    </div>
  );
}

export default ChatUI;
