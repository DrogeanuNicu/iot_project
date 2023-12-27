import React, { Component } from 'react';

import 'bootstrap/dist/css/bootstrap.min.css';
import styles from './styles.module.css';

class SwitchButton extends Component {
  constructor(props) {
    super(props);
    this.name = this.props.name;
  }

  // handleSwitchToggle = () => {

  //   this.setState((prevState) => ({
  //     isOn: !prevState.isOn,
  //   }));

  //   fetch(this.route, {
  //     method: 'POST',
  //     headers: {
  //       'Content-Type': 'application/json',
  //     },
  //     body: JSON.stringify({
  //       isOn: !this.state.isOn,
  //     }),
  //   })
  //     .then(response => {
  //       if (!response.ok) {
  //         throw new Error('Failed to toggle switch');
  //       }
  //       return null;
  //     });
  // };

  render() {
    return (
      <div className="form-check form-switch">
        <label className="form-check-label" htmlFor={this.name + "switch"}>
          {this.name}
        </label>
        <input
          className={`form-check-input ${this.props.isOn ? 'checked' : ''}`}
          type="checkbox"
          id={this.name + "switch"}
          checked={this.props.state}
          readOnly
        />
      </div>
    );
  }
}

export default SwitchButton;
