import React, { Component } from 'react';
import TableRow from "../../components/TableRow"
import 'bootstrap/dist/css/bootstrap.min.css';
import styles from './styles.module.css';

class Table extends Component {
    constructor(props) {
        super(props);
        this.labels = this.props.labels;
        this.limits = this.props.limits;
        this.state = {
            data: props.data,
        };
    }

    generateTable(data, labels, limits) {
        if (data === null) {
            return null;
        }

        const rows = [];
        for (const [property, value] of Object.entries(labels)) {
            if (property === "time") {
                rows.push(
                    <TableRow rowKey={property} name={value.name} data={data[property]} min={null} max={null}></TableRow>
                );
            } else {
                rows.push(
                    <TableRow rowKey={property} name={value.name} data={data[property]} min={limits[property].min} max={limits[property].max}></TableRow>
                );
            }
        }
        return rows;
    }

    render() {
        return (
            <table className="table table-bordered table-striped">
                <thead>
                    <tr>
                        <th>Property</th>
                        <th>Value</th>
                        <th>Min</th>
                        <th>Max</th>
                    </tr>
                </thead>
                <tbody>{this.generateTable(this.props.data, this.labels, this.limits)}</tbody>
            </table>
        );
    }
}

export default Table;
