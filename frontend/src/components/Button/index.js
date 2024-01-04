import React from 'react';

class Button extends React.Component {
  constructor(props) {
    super(props);
    this.text = this.props.text;
    this.route = this.props.route;
  }

  handleDownload = () => {
    fetch(this.route, {
      method: 'GET',
    })
    .then(response => {
      // Check if the response status is OK (200)
      if (response.ok) {
        // Trigger the download
        response.blob().then(blob => {
          const url = window.URL.createObjectURL(new Blob([blob]));
          const link = document.createElement('a');
          link.href = url;
          link.setAttribute('download', 'data.csv');
          document.body.appendChild(link);
          link.click();
          document.body.removeChild(link);
        });
      } else {
        console.error('Error downloading file:', response.statusText);
      }
    })
    .catch(error => {
      console.error('Fetch error:', error);
    });
  }

  render() {
    return (
      <button onClick={this.handleDownload}>
        {this.text}
      </button>
    );
  }
}

export default Button;
