class DataFetcher {
    static async fetchData() {
        try {
            const response = await fetch('/api/data');
            const json_response = await response.json();
            return json_response; // Return the fetched data
        } catch (error) {
            console.error(error);
            return null; // Return null or handle the error as needed
        }
    }
}

export default DataFetcher;
