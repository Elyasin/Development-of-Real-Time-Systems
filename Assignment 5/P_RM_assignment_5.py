"""
Partitionned RM using PartitionedScheduler.
"""
from simso.core.Scheduler import SchedulerInfo
from simso.utils import PartitionedScheduler
from simso.schedulers import scheduler

@scheduler("simso.schedulers.P_RM")
class P_RM_assignment_5(PartitionedScheduler):
    def init(self):
        PartitionedScheduler.init(
            self, SchedulerInfo("simso.schedulers.RM_mono"))

    def packer(self):
        # First Fit RM
        cpus = [[cpu, 0, 0] for cpu in self.processors]
        for task in self.task_list:
            j = -1

            # Find first the processor with utilization lower than U_rm(n + 1)
            # where n is the number of tasks affected to cpu
            for i, c in enumerate(cpus):
                u = c[1] + float(task.wcet) / task.period
                u_rm = (c[2]+1) * (2**(1/float((c[2]+1))) - 1)
                if u <= u_rm:
                    j = i
                    break

            if j == -1:
                return False

            # Affect it to the task.
            self.affect_task_to_processor(task, cpus[j][0])

            # Update utilization.
            cpus[j][1] += float(task.wcet) / task.period

            # Update number of tasks on cpu
            cpus[j][2] += 1
        return True
