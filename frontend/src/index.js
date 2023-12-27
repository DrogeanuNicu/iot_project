import React from 'react';
import ReactDOM from 'react-dom/client';
import Main from './pages/Main';
import DataFetcher from './services/DataFetcher';

async function initializeApp() {
    const data = await DataFetcher.fetchData('api/data');
    const limits = await DataFetcher.fetchData('api/limits');

    const root = ReactDOM.createRoot(document.getElementById('root'));
    root.render(
        <React.StrictMode>
            <Main data={data} limits={limits} />
        </React.StrictMode>
    );
}

initializeApp();
