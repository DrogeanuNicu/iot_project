class DataFetcher {
    static async fetchData(route) {
        try {
            const response = await fetch(route);
            const json_response = await response.json();
            return json_response;
        } catch (error) {
            console.error(error);
            return null;
        }
    }
}

export default DataFetcher;
