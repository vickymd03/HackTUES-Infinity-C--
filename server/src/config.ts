import { readFileSync } from 'fs';

const file = readFileSync('config.json', 'utf8');

const config = JSON.parse(file);
export default config;
