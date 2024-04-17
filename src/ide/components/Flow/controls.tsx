import { Button, ButtonGroup } from 'react-bootstrap';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { useNodeStore } from '../../../core/contexts/NodeStore';
import { memo } from 'react';

const Controls = memo(() => {
  return (
    <ButtonGroup className="controls" style={{ zIndex: 5, position: 'absolute', left: 10, marginTop: 10 }}>
      <Button onClick={() => useNodeStore.getState().run()} variant="outline-secondary" size="sm">
        <FontAwesomeIcon className="text-success" icon={'play-circle'} /> Run
      </Button>
      <Button onClick={() => useNodeStore.getState().stop()} variant="outline-secondary" size="sm">
        <FontAwesomeIcon className="text-danger" icon={'stop-circle'} /> Stop
      </Button>
    </ButtonGroup>
  );
});

export default Controls;
