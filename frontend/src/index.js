import React from 'react';
import ReactDOM from 'react-dom/client';
import Main from './pages/Main';
import DataFetcher from './services/DataFetcher';

async function initializeApp() {
    const firstDataSet = await DataFetcher.fetchData();

    const root = ReactDOM.createRoot(document.getElementById('root'));
    root.render(
        <React.StrictMode>
            <Main data={firstDataSet} />
        </React.StrictMode>
    );
}

initializeApp();
