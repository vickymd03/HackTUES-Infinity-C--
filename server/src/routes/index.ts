import { Router } from 'express';

import botapi from './botapi';

const routes = Router();

routes.use('/botapi', botapi)

export default routes;
