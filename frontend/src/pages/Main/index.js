import React, { Component } from 'react';
import Header from '../../components/Header';
import Table from '../../components/Table';
import LineChart from '../../components/LineChart'

import DataFetcher from '../../services/DataFetcher';

import styles from './styles.module.css';


const labels = {
    "time": { name: "Time", color: "" },
    "temp": { name: "Temperature [°C]", color: "rgb(255, 0, 0)" },
    "hum": { name: "Humidity [g/kg]", color: "rgb(0, 255, 0)" },
    "moist": { name: "Moisture [%]", color: "rgb(0, 0, 255)" }
};

class Main extends Component {
    constructor(props) {
        super(props);
        this.state = {
            data: props.data || null,
        };
    }

    async componentDidMount() {
        this.intervalId = setInterval(async () => {
            const newData = await DataFetcher.fetchData();

            this.setState(() => ({
                data: newData,
            }));

        }, 1000);
    }

    componentWillUnmount() {
        clearInterval(this.intervalId);
    }

    render() {
        const lastDataSet = this.state.data.length > 0 ? this.state.data[this.state.data.length - 1] : null;

        return (
            <div className={styles.MainPage}>
                <Header />
                <div className={styles.ContentContainer}>
                    <div className={styles.LeftContent}>
                        <Table data={lastDataSet} labels={labels}/>
                    </div>
                    <div className={styles.RightContent}>
                        <LineChart data={this.state.data} labels={labels}/>
                    </div>
                </div>
            </div>
        );
    }
}

export default Main;
