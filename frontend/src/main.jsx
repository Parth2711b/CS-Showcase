import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './App.jsx';

// Ye function App.jsx ko index.html ke id="root" mein dal deta hai
ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);
