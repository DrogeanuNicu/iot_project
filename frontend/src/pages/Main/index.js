import React, { Component } from 'react';
import Header from '../../components/Header';
import Table from '../../components/Table';
import LineChart from '../../components/LineChart'
import SwitchButton from '../../components/SwitchButton';
import Button from '../../components/Button';

import DataFetcher from '../../services/DataFetcher';

import styles from './styles.module.css';


const labels = {
    "time": { name: "Time", color: "" },
    "temp": { name: "Temperature [°C]", color: "rgb(95, 158, 200)" },
    "hum": { name: "Humidity [g/kg]", color: "rgb(255, 51, 51)" },
    "moist": { name: "Moisture [%]", color: "rgb(34, 180, 34)" }
};

class Main extends Component {
    constructor(props) {
        super(props);
        this.limits = this.props.limits;
        this.state = {
            data: props.data || null,
        };
    }

    async componentDidMount() {
        this.intervalId = setInterval(async () => {
            const newData = await DataFetcher.fetchData('api/data');

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
                        <Table data={lastDataSet} labels={labels} limits={this.limits} />
                        <div>
                            <SwitchButton name="Fan" state={(lastDataSet.fan !== 0)} />
                            <SwitchButton name="Pump" state={(lastDataSet.pump !== 0)} />
                        </div>
                        <div>
                            <Button text="Download" route="/api/download"/>
                        </div>
                    </div>
                    <div className={styles.RightContent}>
                        <LineChart data={this.state.data} labels={labels} />
                    </div>
                </div>
            </div>
        );
    }
}

export default Main;
