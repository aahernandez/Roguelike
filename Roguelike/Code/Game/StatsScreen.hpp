#pragma once

class StatsScreen
{
public:
	bool m_isDisplayingInentory;
	bool m_isEquippingItem;
	bool m_isDroppingItem;
	bool m_isUnequippingItem;
	Stats m_bonusEquipmentStats;

	StatsScreen();
	~StatsScreen();

	void Initialize();
	void Update(float deltaSeconds);
	void Render() const;
	void RenderBaseStats() const;
	void RenderModifiedStats() const;
	void RenderEquipment() const;
	void RenderHealth() const;
	void RenderInventory() const;
	void Destroy();

	void KeyDown();
};