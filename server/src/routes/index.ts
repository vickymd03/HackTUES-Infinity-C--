import { Router } from 'express';

import botapi from './botapi';
import webinterface from './webinterface';

const routes = Router();

routes.use('/botapi', botapi);
routes.use('/webinterface', webinterface);

export default routes;
