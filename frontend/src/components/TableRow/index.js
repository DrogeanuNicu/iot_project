import React, { Component } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';

class TableRow extends Component {
    constructor(props) {
        super(props);
        this.state = {
            rowKey: props.rowKey,
            name: props.name,
            data: props.data,
            min: props.min,
            max: props.max,
        };
    }

    handleKeyPress = (property, limit, e) => {
        if (e.key === 'Enter') {
            fetch(`/api/set/${property}/${limit}`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    value: e.target.value,
                }),
            })
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Failed to update limit!');
                    }
                    return null;
                });
        }
    };

    handleMinChange(value) {
        this.setState({ min: value });
    };

    handleMaxChange(value) {
        this.setState({ max: value });
    };

    render() {
        if (this.state.min == null || this.state.max == null) {
            return (
                <tr key={this.props.rowKey}>
                    <td >{this.props.name}</td>
                    <td >{this.props.data}</td>
                    <td ></td>
                    <td ></td>
                </tr>
            );
        } else {
            return (
                <tr key={this.props.rowKey}>
                    <td>{this.props.name}</td>
                    <td>{this.props.data}</td>
                    <td>
                        <input
                            id={this.props.rowKey + "_min"}
                            type="number"
                            value={this.state.min}
                            onChange={(e) => this.handleMinChange(e.target.value)}
                            onKeyDown={(e) => this.handleKeyPress(this.props.rowKey, "min", e)}
                        />
                    </td>
                    <td>
                        <input
                            id={this.props.rowKey + "_max"}
                            type="number"
                            value={this.state.max}
                            onChange={(e) => this.handleMaxChange(e.target.value)}
                            onKeyDown={(e) => this.handleKeyPress(this.props.rowKey, "max", e)}
                        />
                    </td>
                </tr>
            );
        }
    }
}

export default TableRow;
