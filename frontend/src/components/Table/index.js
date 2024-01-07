import React, { Component } from 'react';
import TableRow from "./TableRow"
import 'bootstrap/dist/css/bootstrap.min.css';

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
            <div>
                <table className="table table-bordered table-striped" style={{ tableLayout: 'fixed' }}>
                    <thead className="thead-dark">
                        <tr>
                            <th style={{ width: '25%' }}>Property</th>
                            <th style={{ width: '25%' }}>Value</th>
                            <th style={{ width: '25%' }}>Min</th>
                            <th style={{ width: '25%' }}>Max</th>
                        </tr>
                    </thead>
                    <tbody>{this.generateTable(this.props.data, this.labels, this.limits)}</tbody>
                </table>
            </div>
        );
    }
}

export default Table;
