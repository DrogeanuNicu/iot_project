import React, { Component } from 'react';
import styles from './styles.module.css';

class Header extends Component {
  render() {
    return (
      <header className={styles.Header}>
        <h1>Plant Monitoring System</h1>
      </header>
    );
  }
}

export default Header;