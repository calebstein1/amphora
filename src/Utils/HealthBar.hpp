#ifndef AMPHORA_HEALTHBAR_H
#define AMPHORA_HEALTHBAR_H

#include <vector>

#include "engine/amphora.h"

class HealthBar {
private:
    std::vector<AmphoraImage *> health_bar;
    int current_health;
public:
    explicit HealthBar(int default_health = 3) {
	    current_health = 0;
	    for (int i = 0; i < default_health; i++) {
		    increase_health();
	    }
    }

    void increase_health() {
	    current_health++;
	    health_bar.push_back(nullptr);
	    Amphora_CreateSprite(&health_bar.back(), "Objects", -96.0f - (float) (32 * (health_bar.size() - 1)), 24,
				 2, false, true, 1000);
	    Amphora_AddFrameset(health_bar.back(), "Default", nullptr, 63, 0, 16, 16, 0, 0, 1, 0);
    }

    void decrease_health() {
	    if (current_health == 0) return;

	    current_health--;
	    Amphora_FreeSprite(&health_bar.back());
	    health_bar.pop_back();
    }

    [[nodiscard]] int get_health() const {
	    return current_health;
    }

    void set_health(int health) {
	    if (health < current_health) {
		    for (int i = 0; i < current_health - health - 1; i++) {
			    decrease_health();
		    }
	    } else if (health > current_health) {
		    for (int i = 0; i < health - current_health - 1; i++) {
			    increase_health();
		    }
	    }
    }
};

#endif /* AMPHORA_HEALTHBAR_H */
