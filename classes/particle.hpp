#pragma once

class particle {
    private:
        point2 pos, box;
        int dirExecTime, dirChangeTime, direction, gravity;
        point3 rgb;
        bool fallen;

    public:
        particle() : fallen(false) {}
        particle(point2 pos, point3 rgb, int dirExecTime, int dirChangeTime, int direction, int gravity, point2 box) : pos(pos), rgb(rgb), dirExecTime(dirExecTime), dirChangeTime(dirChangeTime), direction(direction), gravity(gravity), box(box), fallen(false) {}

        point2& getPos() { return this->pos; }
        int& getDirExecTime() { return this->dirExecTime; }
        int& getDirChangeTime() { return this->dirChangeTime; }
        int& getDirection() { return this->direction; }
        int& getGravity() { return this->gravity; }
        point2& getBox() { return this->box; }
        point3& getRGB() { return this->rgb; }
        bool& getFallen() { return this->fallen; }

};
