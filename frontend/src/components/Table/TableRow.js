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
            try {
                let value = parseInt(e.target.value, 10);

                if (isNaN(value)) {
                    throw new Error('Please enter a valid number.');
                }

                if (limit === "min") {
                    if (value > this.state.max) {
                        throw new Error("Min can't be higher than max!");
                    }
                } else if (limit === "max") {
                    if (value < this.state.min) {
                        throw new Error("Max can't be lower than min!");
                    }
                }

                fetch(`/api/set/${property}/${limit}`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        value: value,
                    }),
                })
                    .then(response => {
                        if (!response.ok) {
                            throw new Error('Failed to update limit!');
                        }
                        return null;
                    });

            }
            catch (error) {
                if (limit === 'min') {
                    this.setState({ minError: error.message, maxError: null });
                } else if (limit === 'max') {
                    this.setState({ maxError: error.message, minError: null });
                }
            }
        }
    };

    handleMinChange(value) {
        this.setState({ min: value, minError: null, maxError: null });
    };

    handleMaxChange(value) {
        this.setState({ max: value, minError: null, maxError: null });
    };

    render() {
        if (this.state.min == null || this.state.max == null) {
            return (
                <tr key={this.props.rowKey}>
                    <td style={{ width: '25%' }}>{this.props.name}</td>
                    <td style={{ width: '25%' }}>{this.props.data}</td>
                    <td style={{ width: '25%' }}></td>
                    <td style={{ width: '25%' }}></td>
                </tr>
            );
        } else {
            return (
                <tr key={this.props.rowKey}>
                    <td style={{ width: '25%' }}>{this.props.name}</td>
                    <td style={{ width: '25%' }}>{this.props.data}</td>
                    <td style={{ width: '25%' }}>
                        <input
                            id={this.props.rowKey + "_min"}
                            type="number"
                            value={this.state.min}
                            onChange={(e) => this.handleMinChange(e.target.value)}
                            onKeyDown={(e) => this.handleKeyPress(this.props.rowKey, "min", e)}
                            style={{ width: '100%' }}
                        />
                        {this.state.minError && <div style={{ color: 'red' }}>{this.state.minError}</div>}
                    </td>
                    <td style={{ width: '25%' }}>
                        <input
                            id={this.props.rowKey + "_max"}
                            type="number"
                            value={this.state.max}
                            onChange={(e) => this.handleMaxChange(e.target.value)}
                            onKeyDown={(e) => this.handleKeyPress(this.props.rowKey, "max", e)}
                            style={{ width: '100%' }}
                        />
                        {this.state.maxError && <div style={{ color: 'red' }}>{this.state.maxError}</div>}
                    </td>
                </tr>
            );
        }
    }
}

export default TableRow;
