class Process:
    def __init__(self, name, priority, arrival_time, service_time):
        self.name = name
        self.priority = int(priority)
        self.arrival_time = int(arrival_time)
        self.service_time = int(service_time)
        self.remaining_time = self.service_time
        self.isFinished = False
        self.start_time = None
        self.completion_time = None

    def toggle_finished(self, time):
        self.isFinished = True
        self.completion_time = time

    def get_status(self):
        return self.isFinished

    def service(self, serviced_time, current_time):
        if self.start_time is None:
            self.start_time = current_time
        self.remaining_time -= serviced_time
        if self.remaining_time <= 0:
            self.remaining_time = 0
            return True
        return False


class CPU:
    def __init__(self):
        self.current_process = None

    def assign_process(self, process):
        self.current_process = process

    def preempt_if_needed(self, ready_queue):
        candidates = ready_queue[:]
        if self.current_process:
            candidates.append(self.current_process)
        candidates.sort(key=lambda p: (p.remaining_time, p.arrival_time))
        if self.current_process != candidates[0]:
            if self.current_process and self.current_process not in ready_queue:
                ready_queue.append(self.current_process)
            self.current_process = candidates[0]
            ready_queue.remove(self.current_process)

    def execute(self, time):
        if self.current_process:
            finished = self.current_process.service(1, time)
            if finished:
                self.current_process.toggle_finished(time + 1)
                self.current_process = None


def get_user_processes():
    n = int(input("Enter number of processes: "))
    processes = []
    for i in range(n):
        print(f"\nEnter details for Process {i+1}:")
        name = input("  Name: ")
        priority = int(input("  Priority (lower is higher): "))
        arrival = int(input("  Arrival Time: "))
        service = int(input("  Service Time: "))
        processes.append(Process(name, priority, arrival, service))
    return processes


def choose_mode():
    print("\nChoose scheduling algorithm:")
    print("1. FCFS")
    print("2. Non-Preemptive Priority")
    print("3. Shortest Job First (SJF)")
    print("4. Shortest Remaining Time First (SRTF)")
    print("5. Round Robin")
    return input("Enter choice [1-5]: ")


def simulate(process_list, mode, time_quantum=None):
    ready_queue = []
    cpu = CPU()
    time = 0
    timeline = []
    rr_queue = []
    rr_counter = 0

    while True:
        for p in process_list:
            if p.arrival_time == time and not p.get_status():
                if mode == "5":
                    rr_queue.append(p)
                else:
                    ready_queue.append(p)

        if mode == "1":  # FCFS
            if not cpu.current_process and ready_queue:
                cpu.assign_process(ready_queue.pop(0))

        elif mode == "2":  # Non-Preemptive Priority
            if not cpu.current_process and ready_queue:
                ready_queue.sort(key=lambda p: p.priority)
                cpu.assign_process(ready_queue.pop(0))

        elif mode == "3":  # SJF
            if not cpu.current_process and ready_queue:
                ready_queue.sort(key=lambda p: (p.service_time, p.arrival_time))
                cpu.assign_process(ready_queue.pop(0))

        elif mode == "4":  # SRTF
            if ready_queue or cpu.current_process:
                cpu.preempt_if_needed(ready_queue)

        elif mode == "5":  # Round Robin
            if rr_queue and (not cpu.current_process or rr_counter == 0):
                if cpu.current_process:
                    rr_queue.append(cpu.current_process)
                cpu.assign_process(rr_queue.pop(0))
                rr_counter = time_quantum

        if cpu.current_process:
            timeline.append(cpu.current_process.name)
            finished = cpu.current_process.service(1, time)
            if finished:
                cpu.current_process.toggle_finished(time + 1)
                if mode == "5":
                    rr_counter = 0
                cpu.current_process = None
            elif mode == "5":
                rr_counter -= 1
        else:
            timeline.append("idle")

        if all(p.get_status() for p in process_list):
            break

        time += 1

    # Final summary
    print("\n--- Gantt Chart ---")
    print(' '.join(timeline))

    print("\n--- Process Metrics ---")
    print(f"{'Process':<10}{'Arrival':<8}{'Service':<8}{'Complete':<10}{'Turnaround':<12}{'Waiting':<8}")
    for p in process_list:
        turnaround = p.completion_time - p.arrival_time
        waiting = turnaround - p.service_time
        print(f"{p.name:<10}{p.arrival_time:<8}{p.service_time:<8}{p.completion_time:<10}{turnaround:<12}{waiting:<8}")


### Main Program ###
print("CPU Scheduling script for Solving OS assginment sheets :)")
processes = get_user_processes()
mode = choose_mode()
time_quantum = None
if mode == "5":
      time_quantum = int(input("Enter time quantum for Round Robin: "))
simulate(processes, mode, time_quantum)
print("Simulation complete. Developed by Ahmed Mahmoud (@codelink7)")

