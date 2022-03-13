import { Router } from 'express';
import fileUpload from 'express-fileupload';
import { getCommitedCommandQueue, clearCommitedCommandQueue } from '../commands';

import config from '../config'

const router = Router();

router.use(fileUpload({}));

router.use((req, res, next) => {
    setTimeout(next, config.simulatedLatency);
});

// GET can be cached which we don't want
router.post('/get_and_clear_command_queue', (req, res) => {
    res.send(JSON.stringify(getCommitedCommandQueue()));
    clearCommitedCommandQueue();
})

export default router;
