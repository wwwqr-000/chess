#pragma once

class particle {
    private:
        point2 pos, box;
        int dirExecTime, dirChangeTime, direction;
        point3 rgb;

    public:
        particle() {}
        particle(point2 pos, point3 rgb, int dirExecTime, int dirChangeTime, int direction, point2 box) : pos(pos), rgb(rgb), dirExecTime(dirExecTime), dirChangeTime(dirChangeTime), direction(direction), box(box) {}

        point2& getPos() { return this->pos; }
        int& getDirExecTime() { return this->dirExecTime; }
        int& getDirChangeTime() { return this->dirChangeTime; }
        int& getDirection() { return this->direction; }
        point2& getBox() { return this->box; }
        point3& getRGB() { return this->rgb; }

        ~particle() {
            std::cout << "Particle destroyed\n";
        }

};
