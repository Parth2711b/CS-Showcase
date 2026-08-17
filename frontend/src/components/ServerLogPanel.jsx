import React from 'react';

/**
 * ServerLogPanel Component
 * Demonstrates: Real-time Web Updates (Displaying server-pushed events)
 */
function ServerLogPanel() {
  // TODO: Receive 'logs' array as a prop from App.jsx

  return (
    <div>
      <h3>Server Events (Real-time)</h3>
      <div style={{ fontSize: '0.9em', color: '#333' }}>
        {/* TODO: Map over 'logs' and display them. Highlight DEADLOCK_DETECTED events in red. */}
        <p style={{ color: '#888', fontStyle: 'italic' }}>Waiting for server events...</p>
      </div>
    </div>
  );
}

export default ServerLogPanel;
