import React from 'react';
import { Button, Modal } from 'react-bootstrap';

export default class Donate extends React.Component {
  state = {
    show: false,
  };

  handleClose = () => this.setState({ show: false });
  handleShow = () => this.setState({ show: true });

  render() {
    const { show } = this.state;

    return (
      <Modal show={show} onHide={this.handleClose}>
        <Modal.Header closeButton>
          <Modal.Title>Donate</Modal.Title>
        </Modal.Header>
        <Modal.Body>
          <p>From the founder, Rafael A. Piemontez</p>
          <p>
            Your donation will allow this tool to continue online and also
            contribute to the speed of development of new improvements.
          </p>
          <p>Any contribution is welcome.</p>
          <br />
          <div className="d-grid gap-2">
            <Button variant="outline-primary" size="sm" href="" target="_black">
              By PayPal
            </Button>
            <Button variant="outline-secondary" size="sm">
              By Brazilian PIX: {process.env.PIX_KEY}
            </Button>
          </div>
        </Modal.Body>
        <Modal.Footer>
          <Button variant="secondary" onClick={this.handleClose}>
            Close
          </Button>
        </Modal.Footer>
      </Modal>
    );
  }
}
