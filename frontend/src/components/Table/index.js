import React, { Component } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';

class Table extends Component {
    constructor(props) {
        super(props);
        this.state = {
            data: props.data,
            labels: props.labels
        };
    }

    generateTable(data, labels) {
        if (data === null) {
            return null;
        }

        const rows = [];
        for (const [property, value] of Object.entries(labels)) {
            rows.push(
                <tr key={property}>
                    <td>{value.name}</td>
                    <td>{data[property]}</td>
                </tr>
            );
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
                    </tr>
                </thead>
                <tbody>{this.generateTable(this.props.data, this.props.labels)}</tbody>
            </table>
        );
    }
}

export default Table;
