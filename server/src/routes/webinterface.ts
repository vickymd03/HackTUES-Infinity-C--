import { Router } from 'express';

import { parseCommandFromJson, enqueueCommand, commitQueue } from '../commands';

const router = Router();

router.get('/', (req, res) => {
    res.render('panel.ejs', {});
});

router.post('/command', (req, res) => {
    const command = parseCommandFromJson(req.body);
    if (command == null) res.sendStatus(400);
    enqueueCommand(command);
    res.sendStatus(200);
});

router.post('/commit', (req, res) => {
    commitQueue();
});

export default router;
