import React, { Component } from 'react';
import "chart.js/auto";
import { Line } from "react-chartjs-2";

class LineChart extends Component {
    constructor(props) {
        super(props);
        this.state = {
            data: props.data,
        };
    }

    render() {
        let labels = [];
        let datasets = [];
        for (const [property, value] of Object.entries(this.props.labels)) {
            if (property === "time") {
                labels = this.props.data.map(obj => obj[property])
            } else {
                datasets.push({
                    label: value.name,
                    data: this.props.data.map(obj => obj[property]),
                    fill: false,
                    borderWidth: 2,
                    backgroundColor: value.color,
                    borderColor: value.color,
                    responsive: true
                })
            }
        }

        return <Line
            data={{
                labels: labels,
                datasets: datasets,
            }}
        />;
    }
}

export default LineChart;
