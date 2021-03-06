#include "resolver.h"
#include "../framework/math/mmath.h"
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER
#define _alloca(...) alloca(__VA_ARGS__)
#endif

enum ResolveBase
{
	BASE_MAX = RESOLVEBASECOUNT
};

RandomResolver::RandomResolver()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
		Initialize(i);
}

void RandomResolver::PreRun()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
		if (!init[i])
			Initialize(i);
}

float RandomResolver::ResolvePlayer(int id)
{
	if (lastID[id] < 0) {
		int* ang = queue[id].Run(1);

		if (!ang && shotCount[id]++ > RESOLVE_BRUTEFORCE_SHOTS) {
			shotCount[id] = 0;
			Requeue(id);
			ang = queue[id].Run(1);
		}

		if (!ang)
			lastID[id] = (shotCount[id] * RESOLVE_BRUTEFORCE_STEP / RESOLVE_ANGLE_STEP + (totalShots[id]++) / RESOLVE_BRUTEFORCE_SHOTS) % RESOLVE_ANGLE_COUNT;
		else
			lastID[id] = *ang;
	}

	prevID[id] = lastID[id];
	int resolveBase = lastID[id] / RESOLVE_ANGLE_COUNT;
	return baseOffsets[id][resolveBase] + NormalizeFloat(RESOLVE_ANGLE_STEP * (lastID[id] % RESOLVE_ANGLE_COUNT), offsetRanges[resolveBase][0], offsetRanges[resolveBase][1]);
}

void RandomResolver::ProcessHit(int id, float angle)
{
	for (int i = 0; i < RESOLVEBASECOUNT; i++) {
		int ID = (int)(NormalizeFloat(angle - baseOffsets[id][i], 0, 360) / RESOLVE_ANGLE_STEP + 0.5f);
		score[id][i][ID] += 2;
	}

	//We prioritize the last angle base we tried, even though it might not be the right one
	int lid = prevID[id];
	if (lid >= 0) {
		int ID = (int)(NormalizeFloat(angle - baseOffsets[id][lid / RESOLVE_ANGLE_COUNT], 0, 360) / RESOLVE_ANGLE_STEP + 0.5f);
		score[id][lid / RESOLVE_ANGLE_COUNT][ID] = maxScore[id] + 1;
	}

	Requeue(id);
	shotCount[id] = 0;
}

void RandomResolver::ShotFired(int id)
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (lastID[i] >= 0) {
			if (score[i][lastID[i] / RESOLVE_ANGLE_COUNT][lastID[i] % RESOLVE_ANGLE_COUNT] > -2)
				score[i][lastID[i] / RESOLVE_ANGLE_COUNT][lastID[i] % RESOLVE_ANGLE_COUNT]--;
			lastID[i] = -1;
		}
	}
}

void RandomResolver::UpdateBases(int id, float bases[RESOLVEBASECOUNT])
{
	memcpy(baseOffsets[id], bases, sizeof(baseOffsets[id]));
}

void RandomResolver::UpdateOffsets(float offsets[RESOLVEBASECOUNT][2])
{
	memcpy(offsetRanges, offsets, sizeof(offsetRanges));
}

void RandomResolver::Initialize(int id)
{
	for (int i = 0; i < RESOLVE_SCHED_COUNT; i++)
		queue[id].data[i] = -1;

	memset(score[id], 0, sizeof(*score));
	Requeue(id);
	init[id] = true;
	shotCount[id] = 0;
	totalShots[id] = 0;
}

void RandomResolver::Requeue(int id, int insertID)
{
	int count = 0;
	int highestScore = -10000000;

	for (int i = 0; i < ResolveBase::BASE_MAX; i++)
		for (int o = 0; o < RESOLVE_ANGLE_COUNT; o++)
			if (score[id][i][o] > highestScore)
				highestScore = score[id][i][o];

	maxScore[id] = highestScore;

	while (count < RESOLVE_SCHED_COUNT) {
		int cScore = -10000000;
		for (int i = 0; i < ResolveBase::BASE_MAX; i++)
			for (int o = 0; o < RESOLVE_ANGLE_COUNT; o++)
				if (score[id][i][o] >= cScore && (!count || score[id][i][o] < highestScore))
					cScore = score[id][i][o];

		highestScore = cScore;
		int hCount = 0;

		for (int i = 0; i < ResolveBase::BASE_MAX; i++)
			for (int o = 0; o < RESOLVE_ANGLE_COUNT; o++)
				if (score[id][i][o] == cScore)
					hCount++;

		if (!hCount)
			break;

		int* indices = (int*)_alloca(sizeof(int) * hCount);

		int iCount = 0;
		int cnt = 0;

		for (int i = 0; i < ResolveBase::BASE_MAX && iCount < (RESOLVE_SCHED_COUNT - count); i++)
			for (int o = 0; o < RESOLVE_ANGLE_COUNT && iCount < (RESOLVE_SCHED_COUNT - count); o++)
				if (score[id][i][o] == cScore)
					if (rand() % (hCount - (cnt++) + iCount) <= ((RESOLVE_SCHED_COUNT - count) - iCount))
						indices[iCount++] = i * RESOLVE_ANGLE_COUNT + o;

		for (int i = 0; i < iCount; i++) {
			queue[id].burstTime[count] = 1;
			queue[id].priority[count] = count;
			queue[id].data[count++] = indices[i];
		}
	}

	if (insertID >= 0) {
		int id = RESOLVE_SCHED_COUNT - 1;
		for (int i = 0; i < RESOLVE_SCHED_COUNT; i++) {
			if (queue[id].data[i] == insertID) {
				id = i;
				break;
			}
		}
		queue[id].data[id] = insertID;
		queue[id].priority[id] = -1;
	}

	queue[id].Sort();
}
