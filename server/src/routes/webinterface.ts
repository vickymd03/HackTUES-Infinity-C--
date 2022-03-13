import { Router, static as expressStatic } from 'express';

import { parseCommandFromJson, enqueueCommand, commitQueue } from '../commands';

const router = Router();

router.use('/static', expressStatic('views'));

router.post('/command', (req, res) => {
    const command = parseCommandFromJson(req.body);
    if (command == null) return res.sendStatus(400);
    enqueueCommand(command);
    console.log('Enqueued', command);
    res.sendStatus(200);
});

router.post('/commit', (req, res) => {
    try {
        commitQueue();
        res.sendStatus(200);
    } catch {
        res.sendStatus(400);
    }
});

export default router;
